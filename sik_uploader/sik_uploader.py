#!/usr/bin/env python3
#
# Serial firmware uploader for the SiK bootloader
#

import argparse
import binascii
import glob
import serial
import sys
import time

if sys.version_info.major < 3:
    print("This tool requires python3")
    sys.exit(1)


class Firmware(object):
    """Loads a firmware file"""

    # parse a single IntelHex line and obtain the byte array and address
    def __parseline(self, line):
        # ignore lines not beginning with :
        if line[0] != ":":
            return
        # parse the header off the line
        hexstr = line.rstrip()[1:-2]
        binstr = binascii.unhexlify(hexstr)
        command = binstr[3]

        # only type 0 and 4 records are interesting
        if command == 4 and binstr[0] == 0x02 and (binstr[1] << 8) + binstr[2] == 0x0000:
            self.upperaddress = (binstr[4] << 8) + binstr[5]
            self.bankingDeteted = True
        elif command == 0:
            address = (binstr[1] << 8) + binstr[2] + (self.upperaddress << 16)
            bytes = bytearray(binstr[4:])
            if self.upperaddress in self.sanity_check:
                self.sanity_check[self.upperaddress][0] += len(bytes)
                if self.sanity_check[self.upperaddress][1] > (binstr[1] << 8) + binstr[2]:
                    self.sanity_check[self.upperaddress][1] = (binstr[1] << 8) + binstr[2]
                if self.sanity_check[self.upperaddress][2] < (binstr[1] << 8) + binstr[2] + len(bytes):
                    self.sanity_check[self.upperaddress][2] = (binstr[1] << 8) + binstr[2] + len(bytes)
            else:
                self.sanity_check[self.upperaddress] = []
                self.sanity_check[self.upperaddress].append(len(bytes))
                self.sanity_check[self.upperaddress].append((binstr[1] << 8) + binstr[2])
                self.sanity_check[self.upperaddress].append((binstr[1] << 8) + binstr[2] + len(bytes))
            self.__insert(address, bytes)

    # insert the byte array into the ranges dictionary, merging as we go
    def __insert(self, address, bytes):
        # look for a range that immediately follows this one
        candidate = address + len(bytes)
        if candidate in self.ranges:
            # found one, remove from ranges and merge it
            nextbytes = self.ranges.pop(candidate)
            bytes.extend(nextbytes)

        # iterate the existing ranges looking for one that precedes this
        for candidate in list(self.ranges.keys()):
            prevlen = len(self.ranges[candidate])
            if (candidate + prevlen) == address:
                self.ranges[candidate].extend(bytes)
                return
        # just insert it
        self.ranges[address] = bytes

    def __init__(self, path):
        self.ranges = dict()
        self.upperaddress = 0x0000
        self.bankingDeteted = False
        self.sanity_check = dict()

        # read the file
        # XXX should have some file metadata here too ...
        f = open(path, "r")
        for line in f:
            self.__parseline(line)

    def code(self):
        return self.ranges


class Uploader(object):
    """Uploads a firmware file to the SiK bootloader"""

    NOP = chr(0x00)
    OK = chr(0x10)
    FAILED = chr(0x11)
    INSYNC = chr(0x12)
    EOC = chr(0x20)
    GET_SYNC = chr(0x21)
    GET_DEVICE = chr(0x22)
    CHIP_ERASE = chr(0x23)
    LOAD_ADDRESS = chr(0x24)
    PROG_FLASH = chr(0x25)
    READ_FLASH = chr(0x26)
    PROG_MULTI = chr(0x27)
    READ_MULTI = chr(0x28)
    PARAM_ERASE = chr(0x29)
    REBOOT = chr(0x30)

    PROG_MULTI_MAX = 32  # 64 causes serial hangs with some USB-serial adapters
    READ_MULTI_MAX = 255
    BANK_PROGRAMING = -1

    def __init__(self, portname, atbaudrate=57600, use_mavlink=False, mavport=0, debug=0):
        print(("Connecting to %s" % portname))
        if use_mavlink:
            from pymavlink import mavutil
            self.port = mavutil.MavlinkSerialPort(portname, 115200, devnum=mavport,
                                                  timeout=3, debug=debug)
        else:
            self.port = serial.Serial(portname, 115200, timeout=3)
        self.atbaudrate = atbaudrate
        self._debug = debug
        self.percent = -1
        if use_mavlink:
            # we can send a bit more on mavlink
            Uploader.PROG_MULTI_MAX = 64

    def debug(self, s, level=1):
        """write some debug text"""
        if self._debug >= level:
            print(s)

    def __send(self, c):
        if type(c) is str:
            self.port.write(bytes(c, 'latin-1'))
            self.debug(("writing", c), 7)
        else:
            self.debug(("writing ", binascii.hexlify(c)), 7)
            self.port.write(c)

    def __recv(self, raise_error=True):
        start_time = time.time()
        c = self.port.read(1)
        if len(c) < 1:
            self.debug("timeout waiting for data (%.2f seconds)" % (time.time() - start_time))
            if raise_error:
                raise RuntimeError("timeout waiting for data")
            return None
        return c.decode('latin-1')

    def __getSync(self, raise_error=True):
        c = self.__recv(raise_error)
        if c is None:
            return False
        if c != self.INSYNC:
            print("c ", type(c), " ", c)
            print("INSYNC ", type(self.INSYNC), " ", self.INSYNC)
            self.debug(("unexpected ", c, " instead of INSYNC 0x", self.INSYNC))
            if raise_error:
                raise RuntimeError(("unexpected ", c, " instead of INSYNC 0x", self.INSYNC))
            return False
        c = self.__recv()
        if c != self.OK:
            self.debug(("unexpected 0x", c, " instead of OK 0x", self.OK))
            if raise_error:
                raise RuntimeError(("unexpected ", c, "instead of OK 0x", self.OK))
            return False
        self.debug("__getSync OK", 2)
        return True

    # attempt to get back into sync with the bootloader
    def __sync(self):
        # send a stream of ignored bytes longer than the longest possible conversation
        # that we might still have in progress
        self.debug("trying __sync")
        self.__send(Uploader.NOP * (Uploader.PROG_MULTI_MAX + 2))
        self.port.flushInput()
        self.__send(Uploader.GET_SYNC
                    + Uploader.EOC)
        self.debug("trying __getSync")
        return self.__getSync(raise_error=False)

    # send the CHIP_ERASE command and wait for the bootloader to become ready
    def __erase(self, erase_params=False):
        self.__send(Uploader.CHIP_ERASE
                    + Uploader.EOC)
        self.__getSync()
        if erase_params:
            print("resetting parameters...")
            self.__send(Uploader.PARAM_ERASE
                        + Uploader.EOC)
            self.__getSync()

    # send a LOAD_ADDRESS command
    def __set_address(self, address, banking):
        if banking:
            if self.BANK_PROGRAMING != address >> 16:
                self.BANK_PROGRAMING = address >> 16
                if self.BANK_PROGRAMING == 0:
                    print("HOME")
                else:
                    print("BANK", self.BANK_PROGRAMING)
            self.__send(Uploader.LOAD_ADDRESS
                        + chr(address & 0xff)
                        + chr((address >> 8) & 0xff)
                        + chr((address >> 16) & 0xff)
                        + Uploader.EOC)
        else:
            self.debug("set addr - no banking")
            self.__send(Uploader.LOAD_ADDRESS
                        + chr(address & 0xff)
                        + chr(address >> 8)
                        + Uploader.EOC)
            self.debug("set addr - done")
        self.__getSync()

    # send a PROG_FLASH command to program one byte
    def __program_single(self, data):
        self.debug("prog single")
        self.__send(Uploader.PROG_FLASH
                    + chr(data)
                    + Uploader.EOC)
        self.__getSync()

    # send a PROG_MULTI command to write a collection of bytes
    def __program_multi(self, data):
        self.debug("Program Multi", 10)
        sync_count = 0
        while len(data) > 0:
            n = len(data)
            if n > Uploader.PROG_MULTI_MAX:
                n = Uploader.PROG_MULTI_MAX
            block = data[:n]
            data = data[n:]
            self.__send(Uploader.PROG_MULTI + chr(n))
            self.__send(block)
            self.__send(Uploader.EOC)
            sync_count += 1
        while sync_count > 0:
            self.debug("sync_count=%u" % sync_count)
            self.__getSync()
            sync_count -= 1

    # verify multiple bytes in flash
    def __verify_multi(self, data):
        self.__send(Uploader.READ_MULTI
                    + chr(len(data))
                    + Uploader.EOC)
        for i in data:
            if self.__recv() != chr(i):
                return False
        self.__getSync()
        return True

    # send the reboot command
    def __reboot(self):
        self.__send(Uploader.REBOOT)

    # split a sequence into a list of size-constrained pieces
    def __split_len(self, seq, length):
        return [seq[i:i + length] for i in range(0, len(seq), length)]

    def total_size(self, code):
        """return total programming size in bytes"""
        total = 0
        for address in list(code.keys()):
            total += len(code[address])
        return total

    def progress(self, count, total):
        """show progress bar"""
        pct = int((100 * count) / total)
        if pct != self.percent:
            self.percent = pct
            hlen = (pct / 2)
            hashes = '#' * int(hlen)
            spaces = ' ' * (50 - int(hlen))
            sys.stdout.write("[%s] %u/%u (%u%%)\r" % (
                hashes + spaces,
                count,
                total,
                pct))
            sys.stdout.flush()
        if count == total:
            print("")

    # upload code
    def __program(self, fw):
        code = fw.code()
        count = 0
        total = self.total_size(code)
        for address in sorted(code.keys()):
            self.debug("address", address)
            self.__set_address(address, fw.bankingDeteted)
            groups = self.__split_len(code[address], Uploader.PROG_MULTI_MAX)
            for bytes in groups:
                self.__program_multi(bytes)
                count += len(bytes)
                self.progress(count, total)
        self.progress(count, total)
        self.debug("done")

    # verify code
    def __verify(self, fw):
        code = fw.code()
        count = 0
        total = self.total_size(code)
        for address in sorted(code.keys()):
            self.__set_address(address, fw.bankingDeteted)
            groups = self.__split_len(code[address], Uploader.READ_MULTI_MAX)
            for bytes in groups:
                if not self.__verify_multi(bytes):
                    raise RuntimeError("Verification failed in group at 0x%x" % address)
                count += len(bytes)
                self.progress(count, total)
        self.progress(count, total)
        self.debug("done")

    def expect(self, pattern, timeout):
        """wait for a pattern with timeout, return True if found, False if not"""
        import re
        prog = re.compile(pattern)
        start = time.time()
        s = ''
        while time.time() < start + timeout:
            b = self.port.read(1)
            if len(b) > 0:
                sys.stdout.write(b.decode('latin-1'))
                s += b.decode('latin-1')
                if prog.search(s) is not None:
                    return True
            else:
                time.sleep(0.01)
        return False

    def send(self, s):
        """write a string to port and stdout"""
        self.port.write(bytes(s, 'latin-1'))
        sys.stdout.write(s)

    def setBaudrate(self, baudrate):
        try:
            self.port.setBaudrate(baudrate)
        except Exception:
            # for pySerial 3.0, which doesn't have setBaudrate()
            self.port.baudrate = baudrate

    def autosync(self):
        """use AT&UPDATE to put modem in update mode"""
        if self.atbaudrate != 115200:
            self.setBaudrate(self.atbaudrate)
        print("Trying autosync")
        self.send('\r\n')
        time.sleep(1.0)
        self.send('+++')
        self.expect('OK', timeout=1.1)
        for i in range(5):
            self.send('\r\nATI\r\n')
            if not self.expect('SiK .* on', timeout=0.5):
                print("Failed to get SiK banner")
                continue
            self.send('\r\n')
            time.sleep(0.2)
            self.port.flushInput()
            self.send('AT&UPDATE\r\n')
            time.sleep(0.7)
            self.port.flushInput()
            if self.atbaudrate != 115200:
                self.setBaudrate(115200)
            print("Sent update command")
            return True
        if self.atbaudrate != 115200:
            self.setBaudrate(115200)
        return False

    # verify whether the bootloader is present and responding
    def check(self):
        for i in range(3):
            try:
                if self.__sync():
                    print("Got sync")
                    return True
                self.autosync()
            except RuntimeError:
                self.autosync()
        return False

    def identify(self):
        self.__send(Uploader.GET_DEVICE
                    + Uploader.EOC)
        board_id = self.__recv()[0]
        board_freq = self.__recv()[0]
        self.__getSync()
        return board_id, board_freq

    def upload(self, fw, erase_params=False):
        print("erasing...")
        self.__erase(erase_params)
        print("programing...")
        self.__program(fw)
        print("verifying...")
        self.__verify(fw)
        print("done.")
        self.__reboot()


if __name__ == '__main__':
    # Parse commandline arguments
    parser = argparse.ArgumentParser(description="Firmware uploader for the SiK radio system.")
    parser.add_argument('--resetparams', action="store_true", help="reset all parameters to defaults")
    parser.add_argument("--baudrate", type=int, default=57600, help='baud rate')
    parser.add_argument('--forceBanking', action="store_true", help="force the programmer to use 24bit addressing")
    parser.add_argument("--mavlink", action='store_true', default=False, help='update over MAVLink')
    parser.add_argument("--mavport", type=int, default=0, help='MAVLink port number')
    parser.add_argument("--debug", type=int, default=0, help='debug level')
    parser.add_argument('--port', required=True, action="store", help="port to upload to")
    parser.add_argument('firmware', action="store", help="Firmware file to be uploaded")
    args = parser.parse_args()

    # Load the firmware file
    fw = Firmware(args.firmware)
    if args.forceBanking:
        fw.bankingDeteted = True

    ports = glob.glob(args.port)
    if not ports:
        print(("No matching ports for %s" % args.port))
        sys.exit(1)
    # Connect to the device and identify it
    for port in glob.glob(args.port):
        print(("uploading to port %s" % port))
        up = Uploader(port, atbaudrate=args.baudrate, use_mavlink=args.mavlink,
                      mavport=args.mavport, debug=args.debug)
        if not up.check():
            print("Failed to contact bootloader")
            sys.exit(1)
        id, freq = up.identify()
        print(("board %x  freq %x" % (ord(id), ord(freq))))
        # CPU's that support banking have the upper bit set in the byte (0x80)
        if (fw.bankingDeteted and (ord(id) & 0x80) != 0x80):
            print("This firmware requires a CPU with banking")
            sys.exit(1)
        if (ord(id) & 0x80) == 0x80:
            fw.bankingDeteted = True
            print("Using 24bit addresses")
        up.upload(fw, args.resetparams)

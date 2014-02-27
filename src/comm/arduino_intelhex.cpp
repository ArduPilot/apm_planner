#include "arduino_intelhex.h"

void ArduinoIntelHex::loadIntelHex(QString filename)
{
    this->resize(1024*1024);

    QFile intelHex(filename);
    intelHex.open(QIODevice::ReadOnly);
    QTextStream in(&intelHex);
    int optionoffset = 0;
    int total = 0;
    int linenum = 0;
    bool endFound = false;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.startsWith(":")) {
            linenum++;
            bool ok;
            int length = QString("0x" + line.mid(1, 2)).toUInt(&ok, 16);
            int positionA = QString("0x" + line.mid(3, 2)).toUInt(&ok, 16);
            int positionB = QString("0x" + line.mid(5, 2)).toUInt(&ok, 16);
            int position = (positionA * 256) + positionB;
            int option = QString("0x" + line.mid(7, 2)).toUInt(&ok, 16);
            int checksum = QString("0x" + line.right(2)).toUInt(&ok, 16);
            unsigned char checksumact = 0;
            for (int i = 0; i < ((line.length() - 3) / 2); i++) {
                checksumact += QString("0x" + line.mid(i * 2 + 1, 2)).toUInt(&ok, 16);
            }

            if (checksum != (0x100 - checksumact) && checksumact != 0) {
                qDebug()<<checksum<<checksumact<<(0x100 - checksumact);
                qWarning()<<"Wrong checksum on "<<linenum;
                this->clear();
                return;
            }

            //Data
            if (option == 0) {
                QByteArray tmp = QByteArray::fromHex(line.mid(9, length * 2).toLatin1());
                total = optionoffset + position + tmp.length();
                this->insert(optionoffset + position, tmp);
            }
            else if (option == 2)
            {
                int optionoffsetA = QString("0x" + line.mid(9, 2)).toUInt(&ok, 16);
                int optionoffsetB = QString("0x" + line.mid(11, 2)).toUInt(&ok, 16);
                optionoffset = ((optionoffsetA * 256) + optionoffsetB) << 4;
            }
            //End
            else if (option == 1) {
                endFound = true;
            }
        }
    }

    if (!endFound) {
        qWarning()<<"No end found";
        this->clear();
        return;
    }

    this->resize(total);
}

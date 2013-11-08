#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include <QVariantMap>
class AP2DataPlotThread : public QThread
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(QObject *parent = 0);
    void loadFile(QString file);
    class Packet
    {
    public:
        Packet(bool valid = true) { isValid = valid; }
        bool isNAK;
        bool isValid;
        QByteArray header;
        QByteArray payload;
        unsigned short payloadid;
        unsigned short length;
        bool haslength;
        bool hasseq;
        unsigned short sequencenum;
    };
private:
    Packet parseBuffer(QByteArray buffer);
    void loadDataFieldsFromValues();
    QString m_fileName;
    int m_badChecksums;
    int m_falseStarts;
    int m_outOfPacketBytes;
    int m_lengthMismatch;
    int m_badLengthHeader;
    int m_badEscapeChar;
    int m_locationIdInfoReq;
    int m_locationIdInfoReply;
   // FEDataPacketDecoder *decoder;
protected:
    void run();
signals:
    void payloadDecoded(QVariantMap map);
    void decoderFailure(QByteArray buffer);
    void incomingDatalogPacket(QByteArray data);
    void done();
public slots:

};

#endif // AP2DATAPLOTTHREAD_H

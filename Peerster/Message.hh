#ifndef __Peerster__Message__
#define __Peerster__Message__

#include <QMap>
#include <QString>
#include <QVariant>

class Message {
private:
    QString origin;
    quint32 seqno;
    QString message;
    QByteArray serializedMessage;
    QByteArray serializeMessage();
public:
    Message(const QString origin, const quint32 seqno, const QString message);
    QString getOrigin();
    quint32 getSeqno();
    QString getMessage();
    QByteArray getSerializedMessage();
    
};

#endif /* defined(__Peerster__Message__) */

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
    Message() {};
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage) : origin(someOrigin), seqno(someSeqno), message(someMessage) {
        serializedMessage = serializeMessage();
    };
    QString getOrigin();
    quint32 getSeqno();
    QString getMessage();
    QByteArray getSerializedMessage();
    
};

#endif /* defined(__Peerster__Message__) */

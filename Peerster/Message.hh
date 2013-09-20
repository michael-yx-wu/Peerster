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
    QByteArray serializeChatMessage();
    QByteArray serializeRouteMessage();
    QByteArray serailizePrivateMessage();
    
public:
    Message() {};
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage = NULL) : origin(someOrigin), seqno(someSeqno), message(someMessage) {
        if (someMessage != NULL) {
            serializedMessage = serializeChatMessage();
        }
        else {
            serializedMessage = serializeRouteMessage();
        }
    };
    QString getOrigin();
    quint32 getSeqno();
    QString getMessage();
    QByteArray getSerializedMessage();
    bool isChatMessage();
    bool isRouteMessage();
    
};

#endif /* defined(__Peerster__Message__) */

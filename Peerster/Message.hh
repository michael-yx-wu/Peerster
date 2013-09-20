#ifndef __Peerster__Message__
#define __Peerster__Message__

#include <QMap>
#include <QString>
#include <QVariant>

class Message {
public:
#pragma mark - Constructors
    
    Message() {};
    Message(const QString someOrigin, const quint32 someSeqno);
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage);
    Message(const QString someDestOrigin, const QString someMessage, quint32 someHopLimit);
    
    void decrementHopLimit();
    bool atHopLimit();
    bool isChatMessage();
    bool isRouteMessage();
    bool isPrivateMessage();
    QString getDestOrigin();
    quint32 getHopLimit();
    QString getOrigin();
    quint32 getSeqno();
    QString getMessage();
    QByteArray getSerializedMessage();
    
private:
    // Keys
    static const QString xOrigin;
    static const QString xSeqNo;
    static const QString xChatText;
    static const QString xDest;
    static const QString xHopLimit;
    
    // Private Fields
    QString origin;
    QString destOrigin;
    quint32 seqno;
    QString message;
    quint32 hopLimit;
    QByteArray serializedMessage;
    bool chatMessage;
    bool routeMessage;
    bool privateMessage;
  
#pragma mark - Message Serialization
    
    QByteArray serializeChatMessage();
    QByteArray serializePrivateMessage();
    QByteArray serializeRouteMessage();
};

#endif /* defined(__Peerster__Message__) */

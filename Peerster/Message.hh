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
    Message(const QString someOrigin, const quint32 someSeqno, const quint32 someIP, const quint16 somePort);
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage);
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage, const quint32 someIP, const quint16 somePort);
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
    void updateLastIP(quint32 someIP);
    void updateLastPort(quint16 somePort);
    
private:
    // Keys
    static const QString xOrigin;
    static const QString xSeqNo;
    static const QString xChatText;
    static const QString xDest;
    static const QString xHopLimit;
    static const QString xLastIP;
    static const QString xLastPort;
    
    // Private Fields
    QString origin;
    QString destOrigin;
    quint32 seqno;
    QString message;
    quint32 hopLimit;
    quint32 lastIP;
    quint16 lastPort;
    QByteArray serializedMessage;
    bool chatMessage;
    bool hasLastIPandPort;
    bool routeMessage;
    bool privateMessage;
  
#pragma mark - Message Serialization
    
    QByteArray serializeChatMessage();
    QByteArray serializePrivateMessage();
    QByteArray serializeRouteMessage();
};

#endif /* defined(__Peerster__Message__) */

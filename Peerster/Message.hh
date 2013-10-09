#ifndef __Peerster__Message__
#define __Peerster__Message__

#include <QMap>
#include <QString>
#include <QVariant>

#include "Constants.hh"

class Message {
public:
#pragma mark - Constructors
    
    Message() {};
    
    // Route Message
    Message(const QString someOrigin, const quint32 someSeqno);
    Message(const QString someOrigin, const quint32 someSeqno, const quint32 someIP, const quint16 somePort);
    
    // Chat Message
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage);
    Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage, const quint32 someIP, const quint16 somePort);
    
    // Private Chat Message
    Message(const QString someDestOrigin, const QString someMessage, const quint32 someHopLimit);
    
    // Block Reply Message
    Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockReply, const QByteArray someData);

    // Block Request Message
    Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockRequest);
    
#pragma mark - Accessor methods
    
    bool atHopLimit();
    void decrementHopLimit();
    void defaultBoolValues();
    QByteArray getBlockReply();
    QByteArray getBlockRequest();
    QByteArray getData();
    QString getDestOrigin();
    quint32 getHopLimit();
    QString getMessage();
    QString getOrigin();
    quint32 getSeqno();
    QByteArray getSerializedMessage();
    bool isBlockReply();
    bool isBlockRequest();
    bool isChatMessage();
    bool isRouteMessage();
    bool isPrivateMessage();
    bool isSearchReply();
    bool isSearchRequest();
    
private:
    // Private Fields
    QByteArray blockReply;
    QByteArray blockRequest;
    QByteArray data;
    QString destOrigin;
    quint32 hopLimit;
    quint32 lastIP;
    quint16 lastPort;
    QString message;
    QString origin;
    quint32 seqno;
    QByteArray serializedMessage;
    bool blockReplyMessage;
    bool blockRequestMessage;
    bool chatMessage;
    bool hasLastIPandPort;
    bool routeMessage;
    bool privateMessage;
    bool searchReplyMessage;
    bool searchRequestMessage;
    
#pragma mark - Message Serialization
    
    QByteArray serializeChatMessage();
    QByteArray serializePrivateMessage();
    QByteArray serializeRouteMessage();
    QByteArray serializeBlockReplyMessage();
    QByteArray serializeBlockRequestMessage();
};

#endif

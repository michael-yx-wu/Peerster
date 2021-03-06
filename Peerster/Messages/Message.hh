#ifndef __Peerster__Message__
#define __Peerster__Message__

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVariant>

#include "../Constants.hh"

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
    Message(const QString someOrigin, const QString someDest, const QString someMessage, const quint32 someHopLimit);
    
#pragma mark - Accessor methods
    
    bool atHopLimit();
    void decrementHopLimit();
    void defaultBoolValues();
    QByteArray getBlockReply();
    QByteArray getBlockRequest();
    quint32 getBudget();
    QByteArray getData();
    QString getDest();
    quint32 getHopLimit();
    QVariantList getMatchIDs();
    QVariantList getMatchNames();
    QString getMessage();
    QString getOrigin();
    QString getSearchRequest();
    QString getSearchReply();
    quint32 getSeqno();
    QByteArray getSerializedMessage();
    
    bool isAudioMessage();
    bool isBlockReply();
    bool isBlockRequest();
    bool isChatMessage();
    bool isRouteMessage();
    bool isPrivateMessage();
    bool isSearchReply();
    bool isSearchRequest();
    
protected:
    QByteArray blockReply;
    QByteArray blockRequest;
    quint32 budget;
    QByteArray data;
    QString dest;
    quint32 hopLimit;
    quint32 lastIP;
    quint16 lastPort;
    QVariantList matchIDs;
    QVariantList matchNames;
    QString message;
    QString origin;
    QString searchRequest;
    QString searchReply;
    quint32 seqno;
    QByteArray serializedMessage;
    bool audioMessage;
    bool blockReplyMessage;
    bool blockRequestMessage;
    bool chatMessage;
    bool dhKeyMessage;
    bool hasLastIPandPort;
    bool routeMessage;
    bool privateMessage;
    bool searchReplyMessage;
    bool searchRequestMessage;
    
#pragma mark - Message Serialization
    
    QByteArray serializeChatMessage();
    QByteArray serializePrivateMessage();
    QByteArray serializeRouteMessage();
};

#endif

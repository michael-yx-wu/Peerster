#include "Message.hh"

#pragma mark - Constructors

Message::Message(const QString someOrigin, const quint32 someSeqno) {
    defaultBoolValues();
    routeMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    serializedMessage = serializeRouteMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const quint32 someIP, const quint16 somePort) {
    defaultBoolValues();
    hasLastIPandPort = true;
    routeMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeRouteMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage) {
    defaultBoolValues();
    chatMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    message = someMessage;
    serializedMessage = serializeChatMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage, const quint32 someIP, const quint16 somePort) {
    defaultBoolValues();
    hasLastIPandPort = true;
    chatMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    message = someMessage;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeChatMessage();
}

Message::Message(const QString someOrigin, const QString someDest, const QString someMessage, const quint32 someHopLimit) {
    defaultBoolValues();
    privateMessage = true;
    origin = someOrigin;
    dest = someDest;
    message = someMessage;
    hopLimit = someHopLimit;
    serializedMessage = serializePrivateMessage();
}

#pragma mark - Message Serialization

QByteArray Message::serializeChatMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xChatText, message);
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xSeqNo, seqno);
    if (hasLastIPandPort) {
        datapacket.insert(Constants::xLastIP, lastIP);
        datapacket.insert(Constants::xLastPort, lastPort);
    }
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray Message::serializePrivateMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xChatText, message);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray Message::serializeRouteMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xSeqNo, seqno);
    if (hasLastIPandPort) {
        datapacket.insert(Constants::xLastIP, lastIP);
        datapacket.insert(Constants::xLastPort, lastPort);
    }
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;   
    return datagram;
}

# pragma mark - Accessor Methods

bool Message::atHopLimit() {
    return hopLimit == 0;
}

void Message::decrementHopLimit() {
    hopLimit--;
}

void Message::defaultBoolValues() {
    audioMessage = false;
    blockReplyMessage = false;
    blockRequestMessage = false;
    chatMessage = false;
    hasLastIPandPort = false;
    routeMessage = false;
    privateMessage = false;
    searchReplyMessage = false;
    searchRequestMessage = false;
}

QByteArray Message::getBlockReply() {
    return blockReply;
}

QByteArray Message::getBlockRequest() {
    return blockRequest;
}

quint32 Message::getBudget() {
    return budget;
}

QByteArray Message::getData() {
    return data;
}

QString Message::getDest() {
    return dest;
}

quint32 Message::getHopLimit() {
    return hopLimit;
}

QVariantList Message::getMatchIDs() {
    return matchIDs;
}

QVariantList Message::getMatchNames() {
    return matchNames;
}

QString Message::getMessage() {
    return message;
}

QString Message::getOrigin() {
    return origin;
}

QString Message::getSearchRequest() {
    return searchRequest;
}

QString Message::getSearchReply() {
    return searchReply;
}

quint32 Message::getSeqno() {
    return seqno;
}

QByteArray Message::getSerializedMessage() {
    return serializedMessage;
}

bool Message::isAudioMessage() {
    return audioMessage;
}

bool Message::isBlockReply() {
    return blockReplyMessage;
}

bool Message::isBlockRequest() {
    return blockRequestMessage;
}

bool Message::isChatMessage() {
    return chatMessage;
}

bool Message::isRouteMessage() {
    return routeMessage;
}

bool Message::isPrivateMessage() {
    return privateMessage;
}

bool Message::isSearchReply() {
    return searchReplyMessage;
}

bool Message::isSearchRequest() {
    return searchRequestMessage;
}



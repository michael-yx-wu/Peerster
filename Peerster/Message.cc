#include <QDebug>
#include "Message.hh"

#pragma mark - Constructors

const QString Message::xOrigin = "Origin";
const QString Message::xSeqNo = "SeqNo";
const QString Message::xChatText = "ChatText";
const QString Message::xDest = "Dest";
const QString Message::xHopLimit = "HopLimit";
const QString Message::xLastIP = "LastIP";
const QString Message::xLastPort = "LastPort";
const QString Message::xBlockRequest = "BlockRequest";
const QString Message::xBlockReply = "BlockReply";
const QString Message::xData = "Data";

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

Message::Message(const QString someDestOrigin, const QString someMessage, const quint32 someHopLimit) {
    defaultBoolValues();
    privateMessage = true;
    destOrigin = someDestOrigin;
    message = someMessage;
    hopLimit = someHopLimit;
    serializedMessage = serializePrivateMessage();
}

#pragma mark - Block Reply Constructors

Message::Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockReply, const QByteArray someData) {
    defaultBoolValues();
    blockReplyMessage = true;
    origin = someOrigin;
    destOrigin = someDestOrigin;
    hopLimit = someHopLimit;
    blockReply = someBlockReply;
    data = someData;
    serializedMessage = serializeBlockReplyMessage();
}

Message::Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockReply, const QByteArray someData, const quint32 someIP, const quint16 somePort) {
    defaultBoolValues();
    blockReplyMessage = true;
    hasLastIPandPort = true;
    origin = someOrigin;
    destOrigin = someDestOrigin;
    hopLimit = someHopLimit;
    blockReply = someBlockReply;
    data = someData;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeBlockReplyMessage();
}

#pragma mark - Block Request Constructors

Message::Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockRequest) {
    defaultBoolValues();
    blockRequestMessage = true;
    origin = someOrigin;
    destOrigin = someDestOrigin;
    hopLimit = someHopLimit;
    blockRequest = someBlockRequest;
    serializedMessage = serializeBlockRequestMessage();
}

Message::Message(const QString someOrigin, const QString someDestOrigin, const quint32 someHopLimit, const QByteArray someBlockRequest, const quint32 someIP, const quint16 somePort) {
    defaultBoolValues();
    blockRequestMessage = true;
    hasLastIPandPort = true;
    origin = someOrigin;
    destOrigin = someDestOrigin;
    hopLimit = someHopLimit;
    blockRequest = someBlockRequest;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeBlockRequestMessage();
}

#pragma mark - Message Serialization

QByteArray Message::serializeChatMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xChatText, message);
    datapacket.insert(xOrigin, origin);
    datapacket.insert(xSeqNo, seqno);
    if (hasLastIPandPort) {
        datapacket.insert(xLastIP, lastIP);
        datapacket.insert(xLastPort, lastPort);
    }
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray Message::serializePrivateMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xDest, destOrigin);
    datapacket.insert(xChatText, message);
    datapacket.insert(xHopLimit, hopLimit);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray Message::serializeRouteMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xOrigin, origin);
    datapacket.insert(xSeqNo, seqno);
    if (hasLastIPandPort) {
        datapacket.insert(xLastIP, lastIP);
        datapacket.insert(xLastPort, lastPort);
    }
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;   
    return datagram;
}

QByteArray Message::serializeBlockReplyMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xOrigin, origin);
    datapacket.insert(xDest, destOrigin);
    datapacket.insert(xHopLimit, hopLimit);
    datapacket.insert(xBlockReply, blockReply);
    datapacket.insert(xData, data);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray Message::serializeBlockRequestMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xOrigin, origin);
    datapacket.insert(xDest, destOrigin);
    datapacket.insert(xHopLimit, hopLimit);
    datapacket.insert(xBlockRequest, blockRequest);
    if (hasLastIPandPort) {
        datapacket.insert(xLastIP, lastIP);
        datapacket.insert(xLastPort, lastPort);
    }
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

# pragma mark - Other Methods

bool Message::atHopLimit() {
    return hopLimit == 0;
}

void Message::decrementHopLimit() {
    hopLimit--;
}

void Message::defaultBoolValues() {
    blockReplyMessage = false;
    blockRequestMessage = false;
    chatMessage = false;
    hasLastIPandPort = false;
    routeMessage = false;
    privateMessage = false;
    searchReplyMessage = false;
    searchRequestMessage = false;
}

QString Message::getDestOrigin() {
    return destOrigin;
}

quint32 Message::getHopLimit() {
    return hopLimit;
}

QString Message::getMessage() {
    return message;
}

QString Message::getOrigin() {
    return origin;
}

quint32 Message::getSeqno() {
    return seqno;
}

QByteArray Message::getSerializedMessage() {
    return serializedMessage;
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



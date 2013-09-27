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

Message::Message(const QString someOrigin, const quint32 someSeqno) {
    hasLastIPandPort = false;
    routeMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    serializedMessage = serializeRouteMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const quint32 someIP, const quint16 somePort) {
    hasLastIPandPort = true;
    routeMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeRouteMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage) {
    hasLastIPandPort = false;
    chatMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    message = someMessage;
    serializedMessage = serializeChatMessage();
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage, const quint32 someIP, const quint16 somePort) {
    hasLastIPandPort = true;
    chatMessage = true;
    origin = someOrigin;
    seqno = someSeqno;
    message = someMessage;
    lastIP = someIP;
    lastPort = somePort;
    serializedMessage = serializeChatMessage();
}

Message::Message(const QString someDestOrigin, const QString someMessage, quint32 someHopLimit) {
    hasLastIPandPort = false;
    privateMessage = true;
    destOrigin = someDestOrigin;
    message = someMessage;
    hopLimit = someHopLimit;
    serializedMessage = serializePrivateMessage();
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

# pragma mark - Other Methods

void Message::decrementHopLimit() {
    hopLimit--;
}

bool Message::atHopLimit() {
    return hopLimit == 0;
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

QString Message::getDestOrigin() {
    return destOrigin;
}

quint32 Message::getHopLimit() {
    return hopLimit;
}

QString Message::getOrigin() {
    return origin;
}

quint32 Message::getSeqno() {
    return seqno;
}

QString Message::getMessage() {
    return message;
}

QByteArray Message::getSerializedMessage() {
    return serializedMessage;
}

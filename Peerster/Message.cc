#include "Message.hh"

#pragma mark - Constructors

const QString Message::xOrigin = "Origin";
const QString Message::xSeqNo = "SeqNo";
const QString Message::xChatText = "ChatText";
const QString Message::xDest = "Dest";
const QString Message::xHopLimit = "HopLimit";

Message::Message(const QString someOrigin, const quint32 someSeqno) {
    origin = someOrigin;
    seqno = someSeqno;
    serializedMessage = serializeRouteMessage();
    routeMessage = true;
}

Message::Message(const QString someDestOrigin, const QString someMessage, quint32 someHopLimit) {
    origin = someDestOrigin;
    message = someMessage;
    hopLimit = someHopLimit;
    serializedMessage = serializePrivateMessage();
    privateMessage = true;
}

Message::Message(const QString someOrigin, const quint32 someSeqno, const QString someMessage) {
    origin = someOrigin;
    seqno = someSeqno;
    message = someMessage;
    serializedMessage = serializeChatMessage();
    chatMessage = true;
}

#pragma mark - Message Serialization

QByteArray Message::serializeChatMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(xChatText, message);
    datapacket.insert(xOrigin, origin);
    datapacket.insert(xSeqNo, seqno);
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


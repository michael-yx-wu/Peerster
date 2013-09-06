#include "Message.hh"

QByteArray Message::serializeMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert("ChatText", message);
    datapacket.insert("Origin", origin);
    datapacket.insert("SeqNo", seqno);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
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

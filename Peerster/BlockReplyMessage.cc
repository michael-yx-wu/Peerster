#include "BlockReplyMessage.hh"

BlockReplyMessage::BlockReplyMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QByteArray someBlockReply, const QByteArray someData) {
    defaultBoolValues();
    blockReplyMessage = true;
    origin = someOrigin;
    dest = someDest;
    hopLimit = someHopLimit;
    blockReply = someBlockReply;
    data = someData;
    serializedMessage = serializeBlockReplyMessage();
}

QByteArray BlockReplyMessage::serializeBlockReplyMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    datapacket.insert(Constants::xBlockReply, blockReply);
    datapacket.insert(Constants::xData, data);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

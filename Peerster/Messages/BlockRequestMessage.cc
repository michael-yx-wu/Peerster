#include "BlockRequestMessage.hh"

BlockRequestMessage::BlockRequestMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QByteArray someBlockRequest) {
    defaultBoolValues();
    blockRequestMessage = true;
    origin = someOrigin;
    dest = someDest;
    hopLimit = someHopLimit;
    blockRequest = someBlockRequest;
    serializedMessage = serializeBlockRequestMessage();
}

QByteArray BlockRequestMessage::serializeBlockRequestMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    datapacket.insert(Constants::xBlockRequest, blockRequest);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}
#include "SearchReplyMessage.hh"

SearchReplyMessage::SearchReplyMessage(const QString someOrigin, const QString someDest, const quint32 someHoplimit, const QString someSearchReply, const QVariantList someMatchNames, const QVariantList someMatchIDs) {
    defaultBoolValues();
    searchReplyMessage = true;
    origin = someOrigin;
    dest = someDest;
    hopLimit = someHoplimit;
    searchReply = someSearchReply;
    matchNames = someMatchNames;
    matchIDs = someMatchIDs;
    serializedMessage = serializeSearchReplyMessage();
}

QByteArray SearchReplyMessage::serializeSearchReplyMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    datapacket.insert(Constants::xSearchReply, searchReply);
    datapacket.insert(Constants::xMatchNames, matchNames);
    datapacket.insert(Constants::xMatchIDs, matchIDs);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

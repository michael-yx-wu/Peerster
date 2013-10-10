#include "SearchRequestMessage.hh"

SearchMessage::SearchMessage(const QString someOrigin, const QString someQuery, const quint32 someBudget) {
    defaultBoolValues();
    searchRequestMessage = true;
    origin = someOrigin;
    searchRequest = someQuery;
    budget = someBudget;
    serializedMessage = serializeSearchRequestMessage();
}

QByteArray SearchMessage::serializeSearchRequestMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xSearchRequest, searchRequest);
    datapacket.insert(Constants::xBudget, budget);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

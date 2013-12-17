#include "DHKeyMessage.hh"

//Private Message
DHKeyMessage::DHKeyMessage(const QString origin, const QString dhKeyDest, const quint32 dhKeyHopLimit, const QString pubKey) {
    defaultBoolValues();
    dhKeyMessage = true;
    this->origin = origin;
    this->dest = dhKeyDest;
    this->hopLimit = dhKeyHopLimit;
    this->pubKey = pubKey;
    serializedMessage = serializeDHKeyMessage();
}

QByteArray DHKeyMessage::serializeDHKeyMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    datapacket.insert(Constants::xDHKeyData, pubKey);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}



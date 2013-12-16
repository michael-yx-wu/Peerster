#include "AudioMessage.hh"

AudioMessage::AudioMessage(const QString origin, const QDateTime dateTime, const QByteArray audioData) {
    defaultBoolValues();
    audioMessage = true;
    this->origin = origin;
    this->dateTime = dateTime;
    this->audioData = audioData;
    serializedMessage = serializeAudioMessage();
}

//Private Message
AudioMessage::AudioMessage(const QString audioDest, const quint32 audioHopLimit, QDateTime dateTime, const QByteArray audioData) {
    defaultBoolValues();
    audioMessage = true;
    this->dest = audioDest;
    this->hopLimit = audioHopLimit;
    this->dateTime = dateTime;
    this->audioData = audioData;
    serializedMessage = serializeAudioPrivMessage();
}

QDateTime AudioMessage::getTimestamp() {
    return dateTime;
}

QByteArray AudioMessage::serializeAudioMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xOrigin, origin);
    datapacket.insert(Constants::xTimestamp, dateTime);
    datapacket.insert(Constants::xAudioData, audioData);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

QByteArray AudioMessage::serializeAudioPrivMessage() {
    QMap<QString, QVariant> datapacket;
    datapacket.insert(Constants::xDest, dest);
    datapacket.insert(Constants::xHopLimit, hopLimit);
    datapacket.insert(Constants::xTimestamp, dateTime);
    datapacket.insert(Constants::xAudioData, audioData);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}



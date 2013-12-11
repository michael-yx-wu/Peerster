#include "AudioMessage.hh"

AudioMessage::AudioMessage(const QString origin, const QDateTime dateTime, const QByteArray audioData) {
    defaultBoolValues();
    audioMessage = true;
    this->origin = origin;
    this->dateTime = dateTime;
    this->audioData = audioData;
    serializedMessage = serializeAudioMessage();
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

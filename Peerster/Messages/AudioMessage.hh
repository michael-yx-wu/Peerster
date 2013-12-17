#ifndef __Peerster__AudioMessage__
#define __Peerster__AudioMessage__

#include <QDateTime>
#include "Message.hh"

class AudioMessage : public Message {
public:
    AudioMessage(const QString origin, const QDateTime dateTime, const QByteArray audioData);
    AudioMessage(const QString origin, const QString audioDest, const quint32 audioHopLimit, QDateTime dateTime, const QByteArray audioData);

    QDateTime getTimestamp();
protected:
    QDateTime dateTime;
    QByteArray audioData;
    QByteArray serializeAudioMessage();
    QByteArray serializeAudioPrivMessage();
};

#endif

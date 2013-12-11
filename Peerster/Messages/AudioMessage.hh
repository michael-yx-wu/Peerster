#ifndef __Peerster__AudioMessage__
#define __Peerster__AudioMessage__

#include <QDateTime>
#include "Message.hh"

class AudioMessage : public Message {
public:
    AudioMessage(const QString origin, const QDateTime dateTime, const QByteArray audioData);
    
protected:
    QDateTime dateTime;
    QByteArray audioData;
    QByteArray serializeAudioMessage();
    
};

#endif

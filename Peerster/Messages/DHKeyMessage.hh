#ifndef __Peerster__DHKeyMessage__
#define __Peerster__DHKeyMessage__

#include <QtCrypto>
#include "Message.hh"

class DHKeyMessage : public Message {
public:
    DHKeyMessage(const QString origin, const QString dhKeyDest, const quint32 dhKeyHopLimit, const QString pubKey);
protected:
    QString pubKey;
    QByteArray serializeDHKeyMessage();
};

#endif

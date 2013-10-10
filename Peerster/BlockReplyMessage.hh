#ifndef __Peerster__BlockReplyMessage__
#define __Peerster__BlockReplyMessage__

#include "Message.hh"

class BlockReplyMessage : public Message {
public:
    BlockReplyMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QByteArray someBlockReply, const QByteArray someData);
protected:
    QByteArray serializeBlockReplyMessage();
};

#endif

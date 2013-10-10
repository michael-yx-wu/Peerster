#ifndef __Peerster__BlockRequestMessage__
#define __Peerster__BlockRequestMessage__

#include "./Message.hh"

class BlockRequestMessage : public Message {
public:
    BlockRequestMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QByteArray someBlockRequest);
protected:
    QByteArray serializeBlockRequestMessage();
};

#endif

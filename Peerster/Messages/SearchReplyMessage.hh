#ifndef __Peerster__SearchReplyMessage__
#define __Peerster__SearchReplyMessage__

#include <QVariantList>

#include "./Message.hh"

class SearchReplyMessage : public Message {
    SearchReplyMessage(const QString someOrigin, const QString someDest, const quint32 someHoplimit, const QString someSearchReply, const QVariantList someMatchNames, const QVariantList someMatchIDs);
    
protected:
    QByteArray serializeSearchReplyMessage();
};

#endif

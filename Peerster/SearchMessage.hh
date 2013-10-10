#ifndef __Peerster__SearchMessage__
#define __Peerster__SearchMessage__

#include "Message.hh"

class SearchMessage : public Message {
    SearchMessage(const QString someOrigin, const QString someQuery, const quint32 someBudget);
    
    SearchMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QString someQueryReply, const QVariantList someMatchNames, const QVariantList someMatchIDs);
    
};


#endif

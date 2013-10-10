#ifndef __Peerster__SearchRequestMessage__
#define __Peerster__SearchRequestMessage__

#include "./Message.hh"

class SearchMessage : public Message {
public:
    // Search Request
    SearchMessage(const QString someOrigin, const QString someQuery, const quint32 someBudget);
    
protected:
    QByteArray serializeSearchRequestMessage();

};

#endif

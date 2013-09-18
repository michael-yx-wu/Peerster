#ifndef __Peerster__Peer__
#define __Peerster__Peer__

#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QVariant>

class Peer {
    
public:
    QHostAddress address;
    quint16 port;
    Peer() {};
    Peer(QHostAddress a, quint16 p);
};

#endif

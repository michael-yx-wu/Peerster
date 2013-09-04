//
//  Peer.h
//  Peerster
//
//  Created by Michael Wu on 9/4/13.
//
//

#ifndef __Peerster__Peer__
#define __Peerster__Peer__

#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QVariant>

class Peer {
    
public:
    QHostAddress address;
    quint32 port;
    Peer(QHostAddress a, quint32 p);
};

#endif /* defined(__Peerster__Peer__) */

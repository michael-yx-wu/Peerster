//
//  Peer.cpp
//  Peerster
//
//  Created by Michael Wu on 9/4/13.
//
//

#include "Peer.hh"

Peer::Peer(QHostAddress a, quint16 p) {
    address = a;
    port = p;
}
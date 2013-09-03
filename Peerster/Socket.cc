//
//  Socket.cpp
//  Peerster
//
//  Created by Michael Wu on 9/2/13.
//
//

#include "Socket.hh"

// Atempt to bind to a UDP port in range
bool Socket::bind() {
    for (int p = minport; p <= maxport; p++) {
        if (QUdpSocket::bind(QHostAddress::LocalHost, p)) {
            return true;
        }
    }
    
    qDebug() << "No ports in default range " << minport << "-" << maxport << "available";
    return false;
}
//
//  Socket.h
//  Peerster
//
//  Created by Michael Wu on 9/2/13.
//
//

#ifndef __Peerster__Socket__
#define __Peerster__Socket__

#include <QUdpSocket>

class Socket : public QUdpSocket {
    Q_OBJECT
    
private:
    int minport, maxport;

public:
    Socket(QObject *parent) : QUdpSocket(parent) { };
    
    // Attempt to bind to a UDP port in range
    bool bind();
    
};



#endif /* defined(__Peerster__Socket__) */

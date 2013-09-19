#ifndef __Peerster__PrivateMessagingPanel__
#define __Peerster__PrivateMessagingPanel__

#include <QGroupBox>
#include <QPushButton>
#include <QHostAddress>
#include <QVBoxLayout>

class PrivateMessagingPanel {
public:
    PrivateMessagingPanel() ;
    
    QGroupBox* getOriginBox();
    void updateOrigins(QString origin, QHostAddress address, quint16 port);
    
private:
    QMap<QString, QPair<QHostAddress, quint16> > originMap;
    QGroupBox *originBox;
    QVBoxLayout *originList;
    
};

#endif

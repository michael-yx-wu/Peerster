#ifndef __Peerster__PrivateMessagingPanel__
#define __Peerster__PrivateMessagingPanel__

#include <QButtonGroup>
#include <QGroupBox>
#include <QPushButton>
#include <QHostAddress>
#include <QSignalMapper>
#include <QVBoxLayout>

class PrivateMessagingPanel : QObject {
public:
    PrivateMessagingPanel() ;
    
    QGroupBox* getOriginBox();
    void updateOrigins(QString origin, QHostAddress address, quint16 port);

    public slots:
    void buttonClicked(const QString text);
    
private:
    QGroupBox *originBox;
    QMap<QString, QPair<QHostAddress, quint16> > originMap;
    QSignalMapper *signalMapper;
    QVBoxLayout *originList;
    
};

#endif

#ifndef __Peerster__PrivateMessagingPanel__
#define __Peerster__PrivateMessagingPanel__

#include <QButtonGroup>
#include <QGroupBox>
#include <QPushButton>
#include <QHostAddress>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "PrivateChatDialog.hh"

class PrivateMessagingPanel : public QObject {
    Q_OBJECT

public:
    PrivateMessagingPanel();
    
    QGroupBox* getOriginBox();
    QMap<QString, QPair<QHostAddress, quint16> > getOriginMap();
    void setSocket(QUdpSocket *parentSocket);
    void updateOrigins(QString origin, QHostAddress address, quint16 port, bool isDirectRoute);

    public slots:
    void buttonClicked(QString destinationName);
    void windowClosed(QString destinationName);
    
private:
    QSignalMapper *buttonMapper;
    QGroupBox *originBox;
    QVBoxLayout *originList;
    QMap<QString, QPair<QHostAddress, quint16> > originMap;
    QMap<QString, bool> originDirectIndirectMap;
    QMap<QString, PrivateChatDialog*> privateChatDialogs;
    QSignalMapper *privateChatMapper;
    QUdpSocket *socket;
};

#endif

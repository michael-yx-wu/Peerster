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
    PrivateMessagingPanel(QString hostname);
    
    QGroupBox* getOriginBox();
    QMap<QString, QPair<QHostAddress, quint16> > getOriginMap();
    void setSocket(QUdpSocket *parentSocket);
    bool updateOrigins(QString origin, QHostAddress address, quint16 port, quint32 seqno, bool isDirectRoute);

    public slots:
    void buttonClicked(QString destinationName);
    void windowClosed(QString destinationName);
    
private:
    QString hostName;
    QSignalMapper *buttonMapper;
    QGroupBox *originBox;
    QVBoxLayout *originList;
    QMap<QString, QPair<QHostAddress, quint16> > originMap;
    QMap<QString, QPair<quint32, bool> > originDirectIndirectMap;
    QMap<QString, PrivateChatDialog*> privateChatDialogs;
    QSignalMapper *privateChatMapper;
    QUdpSocket *socket;
};

#endif

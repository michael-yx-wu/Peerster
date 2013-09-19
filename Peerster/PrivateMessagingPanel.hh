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
    void startPrivateChat(QString origin, QHostAddress address, quint16);
    void updateOrigins(QString origin, QHostAddress address, quint16 port);

    public slots:
    void buttonClicked(QString destinationName);
    void windowClosed(QString destinationName);
    
private:
    QGroupBox *originBox;
    QMap<QString, QPair<QHostAddress, quint16> > originMap;
    QMap<QString, PrivateChatDialog*> privateChatDialogs;
    QSignalMapper *buttonMapper;
    QSignalMapper *privateChatMapper;
    QVBoxLayout *originList;
};

#endif

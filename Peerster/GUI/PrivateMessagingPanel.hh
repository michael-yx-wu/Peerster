#ifndef __Peerster__PrivateMessagingPanel__
#define __Peerster__PrivateMessagingPanel__

#include <QButtonGroup>
#include <QGroupBox>
#include <QPushButton>
#include <QHostAddress>
#include <QSignalMapper>
#include <QVBoxLayout>
#include "DHKeyMessage.hh"

#include "PrivateChatDialog.hh"

class PrivateMessagingPanel : public QObject {
    Q_OBJECT

public:
    PrivateMessagingPanel(QString hostname);
    
    QGroupBox* getOriginBox();
    QMap<QString, QPair<QHostAddress, quint16> > getOriginMap();
    void setSocket(QUdpSocket *parentSocket);
    bool updateOrigins(QString origin, QHostAddress address, quint16 port, quint32 seqno, bool isDirectRoute);
    void processAudioMessage(QMap<QString, QVariant> datapacket);
    void processChatMessage(QMap<QString, QVariant> datapacket);
    
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
    
    //Encryption
    QCA::BigInteger p;          //public mod value (same for all clients)
    QCA::BigInteger g;          //public generator value (same for all clients)
    QCA::DLGroup dlGroup;       //public dlGroup (same for all clients)
    QCA::BigInteger y;          //private random value
    QString pubKey;    //public key to be sent
    QMap<QString, QCA::SymmetricKey> keyMap;
    void sendDHKeyMessage(DHKeyMessage message);
    void processDHKeyMessage(QMap<QString, QVariant> datapacket);
};

#endif

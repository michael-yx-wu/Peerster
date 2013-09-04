#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QHostInfo>
#include <QTextEdit>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <iostream>
#include <string>

#include "Socket.hh"
#include "Peer.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    QString hostname;
    int myport;
    QUdpSocket *socket;
    
    public slots:
    void gotReturnPressed();
    void processPendingDatagrams();
    void mongerTimeout();
    
private:
    int minport, maxport;
    bool shouldContinueMongering, timeout;
    std::vector<Peer> peers;
    quint32 messageNo;
    QTextEdit *textview;
    Textbox *textbox;
    QTimer *mongerTimer;
    QMap<QString, QVariant> status;
    QMap<QString, QVariant> messages;
    
    void addPeer(QHostAddress address, quint32 p);
    
    bool bind();
    QByteArray serializeMessage();
    void rumorMonger(QByteArray datagram);
    
    void sendChatMessage(QByteArray datagram, QHostAddress address, int port);
    void sendStatusMessage(QHostAddress address, int port);
    void processRumorMessage(QMap<QString, QVariant> datapacket);
    void processStatusMessage(QMap<QString, QVariant> datapacket);
    
};

#endif // PEERSTER_MAIN_HH

#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QApplication>
#include <QByteArray>
#include <QDialog>
#include <QGroupBox>
#include <QHostInfo>
#include <QMap>
#include <QQueue>
#include <QTextCodec>
#include <QTextEdit>
#include <QTimer>
#include <QUdpSocket>
#include <QVector>
#include <QVBoxLayout>
#include <QtCrypto>

#include "Chatbox.hh"
#include "FilePanel.hh"
#include "MapKeys.hh"
#include "Message.hh"
#include "Messages.hh"
#include "Peer.hh"
#include "PrivateMessagingPanel.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    quint16 myport;
    QHostAddress myIP;
    QString hostname;
    QUdpSocket *socket;
    bool shouldForwardMessages;
    
    void resolvePeer(QString hostPort);
    
    public slots:
    void antiEntropyTimeout();
    void gotReturnPressedChatBox();
    void gotReturnPressedHostBox();
    void lookupHostResults(const QHostInfo &host);
    void mongerTimeout();
    void myIPResults(const QHostInfo &host);
    void processPendingDatagrams();
    void routeMonger();

signals:
    void lookupDone();
    
private:
    // Standard Map Keys
//    static const QString xOrigin;
//    static const QString xSeqNo;
//    static const QString xChatText;
//    static const QString xWant;
//    static const QString xDest;
//    static const QString xHopLimit;
//    static const QString xLastIP;
//    static const QString xLastPort;
    
    // Chat Dialog Constants
    quint16 minport, maxport;
    QTextEdit *textview;
    Chatbox *addHostBox;
    Chatbox *chatbox;
    
    std::vector<Peer> peers;
    std::vector<QHostAddress> foundAddresses;
    Messages messages;
    quint32 messageNo;
    
    // Rumor Mongering
    QMap<QString, QVariant> status;
    QQueue<Message> newMessages;
    
    // Anti-Entropy
    QTimer *antiEntropyTimer;
    
    // Routing
    PrivateMessagingPanel privateMessagingPanel;
    QMap<QString, QPair<QHostAddress, quint16> > routingTable;
    QTimer *routingTimer;

    // File sharing
    FilePanel filePanel;    
    
    bool bind();
    void processRumorMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void processPrivateMessage(QMap<QString, QVariant> datapacket);
    void processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void rumorMonger(Message message);
    void rumorMonger(Message message, QHostAddress address, quint16 port);
    void sendMessage(Message message, QHostAddress address, quint16 port);
    void sendStatusMessage(QHostAddress address, quint16 port);
    void updatePeerList(QHostAddress address, quint16 port);
    void updateOriginButtons(QString origin, QHostAddress address, quint16 port);
    bool updatePrivateMessagingPanel(QString origin, QHostAddress address, quint16 port, quint32 seqno, bool isDirectRoute);
};

#endif
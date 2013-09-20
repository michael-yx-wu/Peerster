#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QGroupBox>
#include <QHostInfo>
#include <QQueue>
#include <QTextEdit>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <QVector>

#include "Messages.hh"
#include "Message.hh"
#include "Peer.hh"
#include "PrivateMessagingPanel.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    quint16 myport;
    QHostAddress myIP;
    QString hostname;
    QUdpSocket *socket;
    
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
    static const QString xOrigin;
    static const QString xSeqNo;
    static const QString xChatText;
    static const QString xWant;
    
    // Chat Dialog Constants
    quint16 minport, maxport;
    QTextEdit *textview;
    Textbox *addHostBox;
    Textbox *chatbox;
    
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
    PrivateMessagingPanel knownOrigins;
    QMap<QString, QPair<QHostAddress, quint16> > routingTable;
    QTimer *routingTimer;
    
    bool bind();
    bool processRumorMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void rumorMonger(QString origin, quint32 seqno, QString message, QHostAddress address, quint16 port);
    void rumorMonger(Message message, QHostAddress address, quint16 port);
    void sendMessage(Message message, QHostAddress address, quint16 port);
    void sendStatusMessage(QHostAddress address, quint16 port);
    void updatePeerList(QHostAddress address, quint16 port);
    void updateOriginButtons(QString origin, QHostAddress address, quint16 port);
    void updatePrivateMessagingPanel(QString origin, QHostAddress address, quint16 port);
};

#endif
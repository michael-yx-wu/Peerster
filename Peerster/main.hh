#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QHostInfo>
#include <QTextEdit>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <QVector>

#include "Messages.hh"
#include "Message.hh"
#include "Peer.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    QString hostname;
    quint16 myport;
    QHostAddress myIP;
    QUdpSocket *socket;
    
    void resolvePeer(QString hostPort);
    
    public slots:
    void antiEntropyTimeout();
    void gotReturnPressed();
    void mongerTimeout();
    void processPendingDatagrams();
    void lookupHostResults(const QHostInfo &host);
    void myIPResults(const QHostInfo &host);
signals:
    void lookupDone();
    
private:
    // Chat Dialog Constants
    quint16 minport, maxport;
    QTextEdit *textview;
    Textbox *chatbox;
    
    std::vector<Peer> peers;
    std::vector<QHostAddress> foundAddresses;
    Messages messages;
    quint32 messageNo;
    
    // Rumor Mongering
    QTimer *mongerTimer;
    QMap<QString, QVariant> status;
    QMap<QString, Message> lastSentMessages; // <host, Message>
    Peer lastTarget;
    
    // Anti-Entropy
    QTimer *antiEntropyTimer;
    
    bool bind();
    bool processRumorMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    void rumorMonger(QString origin, quint32 seqno, QString message, QHostAddress address, quint16 port);
    void rumorMonger(Message message, QHostAddress address, quint16 port);
    void sendChatMessage(Message message, QHostAddress address, quint16 port);
    void sendStatusMessage(QHostAddress address, quint16 port);
    void updatePeerList(QHostAddress address, quint16 port);
    
};

#endif // PEERSTER_MAIN_HH

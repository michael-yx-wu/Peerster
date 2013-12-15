#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
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

#include "Constants.hh"
#include "GUI/Chatbox.hh"
#include "GUI/FilePanel.hh"
#include "GUI/PrivateMessagingPanel.hh"
#include "GUI/VoipPanel.hh"
#include "Messages/Message.hh"
#include "Messages/BlockReplyMessage.hh"
#include "Messages/BlockRequestMessage.hh"
#include "Messages/SearchReplyMessage.hh"
#include "Messages/SearchRequestMessage.hh"
#include "Messages.hh"
#include "Peer.hh"

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
    // Chat Dialog Constants
    quint16 minport, maxport;
    QTextEdit *textview;
    Chatbox *addHostBox;
    Chatbox *chatbox;
    
    std::vector<Peer> peers;
    std::vector<QHostAddress> foundAddresses;
    Messages messages;
    quint32 messageNo;
    
    // Voip Mongering
    /* insert new variables here */
    // <origin, timestamp>
    // if message in table, stop mongering, else
    // add to table, monger to peers
    QMap<QString, QVariant> voipStatus;
    
    // Rumor Mongering
    QMap<QString, QVariant> status;
    QQueue<Message> newMessages;
    
    // Anti-Entropy
    QTimer *antiEntropyTimer;
    
    // Routing & Private Messaging
    PrivateMessagingPanel *privateMessagingPanel;
    QTimer *routingTimer;

    // File sharing
    FilePanel *filePanel;
    
    // Voip
    VoipPanel *voipPanel;
    
    bool bind();
    void processAudioMessage(QMap<QString, QVariant> datapacket);
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
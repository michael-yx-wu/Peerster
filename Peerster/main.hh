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
#include "Peer.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    QString hostname;
    quint16 myport;
    QUdpSocket *socket;
    
    public slots:
    void gotReturnPressed();
    void processPendingDatagrams();
    void mongerTimeout();
    
private:
    // Chat Dialog Constants
    quint16 minport, maxport;
    QTextEdit *textview;
    Textbox *textbox;
    
    std::vector<Peer> peers;
    Messages messages;
    quint32 messageNo;
    
    QTimer *mongerTimer;
    QMap<QString, QVariant> status;
    
    void updatePeerList(QHostAddress address, quint16 port);
    
    bool bind();
    QByteArray serializeMessage(QString message, QString origin, quint32 seqno);

    // Rumor Mongering methods
    void rumorMonger(QByteArray datagram, QHostAddress peer, quint16 port);
    void sendChatMessage(QByteArray datagram, QHostAddress address, quint16 port);
    void sendStatusMessage(QHostAddress address, quint16 port);
    bool processRumorMessage(QMap<QString, QVariant> datapacket);
    void processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort);
    
};

#endif // PEERSTER_MAIN_HH

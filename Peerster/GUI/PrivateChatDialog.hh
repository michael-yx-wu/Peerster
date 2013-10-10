#ifndef __Peerster__PrivateChatDialog__
#define __Peerster__PrivateChatDialog__

#include <QDialog>
#include <QGridLayout>
#include <QHostAddress>
#include <QTextEdit>
#include <QUdpSocket>

#include "Chatbox.hh"
#include "../Messages/Message.hh"

class PrivateChatDialog : public QDialog {
    Q_OBJECT
    
public:
    PrivateChatDialog() {};
    PrivateChatDialog(QString destName, QUdpSocket *parentSocket);
    ~PrivateChatDialog();
    
    void closeEvent(QCloseEvent *event);
    QString getDestinationName();
    void updateDestinationIPandPort(QHostAddress destIP, quint16 destPort);
    
signals:
    void privateChatClosed();
    
    public slots:
    void gotReturnPressedChatBox();
    
private:
    Chatbox *chatbox;
    QHostAddress destinationIP;
    QString destinationName;
    quint16 destinationPort;
    quint32 hopLimit;
    QGridLayout *layout;
    QUdpSocket *socket;
    QTextEdit *textview;
};

#endif

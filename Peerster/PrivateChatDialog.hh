#ifndef __Peerster__PrivateChatDialog__
#define __Peerster__PrivateChatDialog__

#include <QDialog>
#include <QGridLayout>
#include <QHostAddress>
#include <QTextEdit>

#include "Textbox.hh"

class PrivateChatDialog : public QDialog {
    Q_OBJECT
    
public:
    PrivateChatDialog() {};
    PrivateChatDialog(QString destName);
    PrivateChatDialog(QString destName, QHostAddress destIP, quint16 destPort);
    ~PrivateChatDialog();
    void closeEvent(QCloseEvent *event);
    QString getDestinationName();
    void updateDestinationIPandPort(QHostAddress destIP, quint16 destPort);
    
signals:
    void privateChatClosed();
    
private:
    quint16 destinationPort;
    QGridLayout *layout;
    QHostAddress destinationIP;
    QString destinationName;
    QTextEdit *textview;
    Textbox *chatbox;
};

#endif

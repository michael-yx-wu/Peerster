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
    PrivateChatDialog(QString destName, QHostAddress destIP, quint16 destPort);
    ~PrivateChatDialog();
    
private:
    quint16 destinationPort;
    QGridLayout *layout;
    QHostAddress destinationIP;
    QString destinationName;
    QTextEdit *textview;
    Textbox *chatbox;
};

#endif

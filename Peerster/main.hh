#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QHostInfo>
#include <QTextEdit>
#include <QUdpSocket>

#include <iostream>
#include <string>

#include "Socket.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    QString hostname;
//    Socket socket;
    QUdpSocket *socket;
    
    public slots:
    void gotReturnPressed();
    void processPendingDatagrams();
    
private:
    QTextEdit *textview;
    Textbox *textbox;
    int minport, maxport;
    int messageNo;
    
    // Attempt to bind to a UDP port in range
    bool bind();
};

#endif // PEERSTER_MAIN_HH

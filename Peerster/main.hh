#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QHostInfo>
#include <QTextEdit>
#include <QUdpSocket>
#include <iostream>
#include <string>
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    
    public slots:
    void gotReturnPressed();
    
private:
    QTextEdit *textview;
    Textbox *textbox;
    QString hostname;
};

class NetSocket : public QUdpSocket {
	Q_OBJECT
    
public:
	NetSocket();
    
	// Bind this socket to a Peerster-specific default port.
	bool bind();
    public slots:
    void readPendingDatagrams();
    
private:
	int myPortMin, myPortMax;
};

#endif // PEERSTER_MAIN_HH

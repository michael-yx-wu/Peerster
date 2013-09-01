#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QUdpSocket>
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
};

class NetSocket : public QUdpSocket
{
	Q_OBJECT
    
public:
	NetSocket();
    
	// Bind this socket to a Peerster-specific default port.
	bool bind();
    
private:
	int myPortMin, myPortMax;
};

#endif // PEERSTER_MAIN_HH

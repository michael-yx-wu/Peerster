#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QHostInfo>
#include <QTextEdit>
#include <QUdpSocket>
#include <QByteArray>

#include <iostream>
#include <string>

#include "Socket.hh"
#include "Textbox.hh"

class ChatDialog : public QDialog {
	Q_OBJECT
    
public:
    ChatDialog();
    QString hostname;
    QUdpSocket *socket;
    
    public slots:
    void gotReturnPressed();
    void processPendingDatagrams();
    
private:
    int minport, maxport;
    quint32 messageNo;
    QTextEdit *textview;
    Textbox *textbox;
    
    // Map to keep track of latest unseen messages from peers
    QMap<QString, QVariant> status;

    // Serialize textbox text into a QByteArray
    QByteArray serializeMessage();
    
    // Attempt to bind to a UDP port in range
    bool bind();
};

#endif // PEERSTER_MAIN_HH

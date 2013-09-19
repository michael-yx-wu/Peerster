#include "PrivateChatDialog.hh"

PrivateChatDialog::PrivateChatDialog(QString destName) {
    destinationName = destName;
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    chatbox = new Textbox(this);
    
    layout = new QGridLayout();
    layout->addWidget(textview);
    layout->addWidget(chatbox);
    chatbox->setFocus();
    
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(layout);
    setWindowTitle("Private Chat with " + destinationName);
}

PrivateChatDialog::PrivateChatDialog(QString destName, QHostAddress destIP, quint16 destPort) {
    destinationName = destName;
    destinationIP = destIP;
    destinationPort = destPort;
    
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    chatbox = new Textbox(this);

    layout = new QGridLayout();
    layout->addWidget(textview);
    layout->addWidget(chatbox);
    chatbox->setFocus();
    
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(layout);
    setWindowTitle("Private Chat with " + destinationName);
}

PrivateChatDialog::~PrivateChatDialog() {
    delete textview;
    delete chatbox;
    delete layout;
}

void PrivateChatDialog::closeEvent(QCloseEvent *event) {
    emit privateChatClosed();
    QDialog::closeEvent(event);
}

QString PrivateChatDialog::getDestinationName() {
    return destinationName;
}

void PrivateChatDialog::updateDestinationIPandPort(QHostAddress destIP, quint16 destPort) {
    destinationIP = destIP;
    destinationPort = destPort;    
}
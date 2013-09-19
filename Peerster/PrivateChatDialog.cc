#include "PrivateChatDialog.hh"

PrivateChatDialog::PrivateChatDialog(QString destName, QHostAddress destIP, quint16 destPort) {
    destinationName = destName;
    destinationIP = destIP;
    destinationPort = destPort;
    
    setWindowTitle("Private Chat with " + destinationName);
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    chatbox = new Textbox(this);

    layout = new QGridLayout();
    layout->addWidget(textview);
    layout->addWidget(chatbox);
    setLayout(layout);
    chatbox->setFocus();
}

PrivateChatDialog::~PrivateChatDialog() {
    delete textview;
    delete chatbox;
    delete layout;
}
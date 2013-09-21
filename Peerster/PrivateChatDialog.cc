#include "PrivateChatDialog.hh"

PrivateChatDialog::PrivateChatDialog(QString destName, QUdpSocket *parentSocket) {
    destinationName = destName;
    socket = parentSocket;
    hopLimit = 10;
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    chatbox = new Chatbox(this);
    
    layout = new QGridLayout();
    layout->addWidget(textview);
    layout->addWidget(chatbox);
    chatbox->setFocus();
    
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(layout);
    setWindowTitle("Private Chat with " + destinationName);
    
    connect(chatbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressedChatBox()));
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

void PrivateChatDialog::gotReturnPressedChatBox() {
    Message message = Message(destinationName, chatbox->toPlainText(), hopLimit);
    qDebug() << "Writing private message to" << message.getDestOrigin() << destinationIP << ":" << destinationPort;
    
    textview->append(message.getMessage());
    chatbox->clear();
    
    QByteArray datagram = message.getSerializedMessage();
    
    socket->writeDatagram(datagram.data(), datagram.size(), destinationIP, destinationPort);
    qDebug() << "Send Private Message!";
}
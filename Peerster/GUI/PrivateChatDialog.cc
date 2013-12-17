#include "PrivateChatDialog.hh"

PrivateChatDialog::PrivateChatDialog(QString hostName, QString destName,
                                    QUdpSocket *parentSocket,
                                    QMap<QString, QPair<QHostAddress, quint16> >
                                     *originMap) {
    destinationName = destName;
    socket = parentSocket;
    hopLimit = 10;
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    chatbox = new Chatbox(this);
    voipChat = new VoipPanel(hostName, destName, socket, originMap);
    origin = hostName;
    
    layout = new QGridLayout();
    layout->addWidget(textview);
    layout->addWidget(chatbox);
    layout->addWidget(voipChat->getButtonGroupBox());
    chatbox->setFocus();
    
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(layout);
    setWindowTitle("Private Chat with " + destinationName);
    
    connect(chatbox, SIGNAL(enterPressed()), this,
            SLOT(gotReturnPressedChatBox()));
}

PrivateChatDialog::~PrivateChatDialog() {
    delete textview;
    delete chatbox;
    delete layout;
    delete voipChat;
}

void PrivateChatDialog::closeEvent(QCloseEvent *event) {
    emit privateChatClosed();
    QDialog::closeEvent(event);
}

QString PrivateChatDialog::getDestinationName() {
    return destinationName;
}

void PrivateChatDialog::updateDestinationIPandPort(QHostAddress destIP,
                                                   quint16 destPort) {
    destinationIP = destIP;
    destinationPort = destPort;
    voipChat->updateDestinationIPandPort(destIP, destPort);    
}

void PrivateChatDialog::gotReturnPressedChatBox() {
    Message message = Message(origin, destinationName, chatbox->toPlainText(),
                              hopLimit);
    textview->append(message.getMessage());
    chatbox->clear();
    QByteArray datagram = message.getSerializedMessage();
    socket->writeDatagram(datagram.data(), datagram.size(), destinationIP,
                          destinationPort);
}

VoipPanel* PrivateChatDialog::getVoipPanel() {
    return voipChat;
}

void PrivateChatDialog::addTextToPrivateChat(QString text) {
    textview->append(text);
}

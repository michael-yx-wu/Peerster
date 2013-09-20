#include "PrivateMessagingPanel.hh"

PrivateMessagingPanel::PrivateMessagingPanel() {
    originBox = new QGroupBox("Known Origins");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
    buttonMapper = new QSignalMapper(this);
    privateChatMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    connect(privateChatMapper, SIGNAL(mapped(QString)), this, SLOT(windowClosed(QString)));
}

QGroupBox* PrivateMessagingPanel::getOriginBox() {
    return originBox;
}

void PrivateMessagingPanel::setSocket(QUdpSocket *parentSocket) {
    socket = parentSocket;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port) {
    qDebug() << "Updating origin information for: " << origin;
    if (!originMap.contains(origin)) {
        qDebug() << "New button for new origin!";
        QPushButton *originButton = new QPushButton(origin);
        buttonMapper->setMapping(originButton, originButton->text());
        connect(originButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
        originList->addWidget(originButton);
    }
    
    // Update dest
    originMap.insert(origin, qMakePair(address, port));
}

void PrivateMessagingPanel::buttonClicked(QString destinationName) {
    qDebug() << "Starting private chat with " + destinationName;
    PrivateChatDialog *privateChat = new PrivateChatDialog(destinationName, socket);
    privateChatMapper->setMapping(privateChat, privateChat->getDestinationName());
    connect(privateChat, SIGNAL(privateChatClosed()), privateChatMapper, SLOT(map()));
    privateChatDialogs.insert(destinationName, privateChat);
    privateChat->show();
}

void PrivateMessagingPanel::windowClosed(QString destinationName) {
    qDebug() << "Closing private chat with: " + destinationName;
    privateChatDialogs.remove(destinationName);
}


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

QMap<QString, QPair<QHostAddress, quint16> > PrivateMessagingPanel::getOriginMap() {
    return originMap;
}

void PrivateMessagingPanel::setSocket(QUdpSocket *parentSocket) {
    socket = parentSocket;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port, bool isDirectRoute) {
    qDebug() << "Updating origin information for: " << origin;
    qDebug() << address << port;
    if (!originMap.contains(origin)) {
        qDebug() << "New button for new origin!";
        QPushButton *originButton = new QPushButton(origin);
        buttonMapper->setMapping(originButton, originButton->text());
        connect(originButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
        originList->addWidget(originButton);
    }
    
    // If last route to origin was direct route, update only if new route is direct route
    if (originMap.contains(origin) && originDirectIndirectMap.value(origin)) {
        if (isDirectRoute) {
            originMap.insert(origin, qMakePair(address, port));
            originDirectIndirectMap.insert(origin, isDirectRoute);
        }
    }
    else {
        originMap.insert(origin, qMakePair(address, port));
        originDirectIndirectMap.insert(origin, isDirectRoute);
    }
    
    if (privateChatDialogs.contains(origin)) {
        qDebug() << "Updating open private chat window";
        privateChatDialogs.value(origin)->updateDestinationIPandPort(address, port);
    }
}

void PrivateMessagingPanel::buttonClicked(QString destinationName) {
    qDebug() << "Starting private chat with " + destinationName;
    PrivateChatDialog *privateChat = new PrivateChatDialog(destinationName, socket);
    privateChat->updateDestinationIPandPort(originMap.value(destinationName).first, originMap.value(destinationName).second);
    privateChatMapper->setMapping(privateChat, privateChat->getDestinationName());
    connect(privateChat, SIGNAL(privateChatClosed()), privateChatMapper, SLOT(map()));
    privateChatDialogs.insert(destinationName, privateChat);
    privateChat->show();
}

void PrivateMessagingPanel::windowClosed(QString destinationName) {
    qDebug() << "Closing private chat with: " + destinationName;
    privateChatDialogs.remove(destinationName);
}


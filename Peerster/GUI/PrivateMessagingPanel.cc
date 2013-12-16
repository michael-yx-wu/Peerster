#include "PrivateMessagingPanel.hh"

PrivateMessagingPanel::PrivateMessagingPanel(QString hostname) {
    hostName = hostname;
    originBox = new QGroupBox("Private Messaging");
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

bool PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port, quint32 seqno, bool isDirectRoute) {
    
    // Create a new button for previously unkonwn origins
    if (!originMap.contains(origin)) {
        QPushButton *originButton = new QPushButton(origin);
        buttonMapper->setMapping(originButton, originButton->text());
        connect(originButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
        originList->addWidget(originButton);
    }
    
    // If last route to origin was direct route, update only if new route is direct route
    // or if new route has higher seqno
    if (origin.contains(origin)) {
        if (seqno >= originDirectIndirectMap.value(origin).first) {
            if (originDirectIndirectMap.value(origin).second && isDirectRoute) {
                qDebug() << "Updating origin information for: " << origin;
                qDebug() << address;
                originMap.insert(origin, qMakePair(address, port));
                originDirectIndirectMap.insert(origin, qMakePair(seqno, isDirectRoute));
                if (privateChatDialogs.contains(origin)) {
                    qDebug() << "Updating open private chat window";
                    privateChatDialogs.value(origin)->updateDestinationIPandPort(address, port);
                }
                return true;
            }
            else if (!originDirectIndirectMap.value(origin).second && seqno > originDirectIndirectMap.value(origin).first) {
                qDebug() << "Updating origin information for: " << origin;
                qDebug() << address;
                originMap.insert(origin, qMakePair(address, port));
                originDirectIndirectMap.insert(origin, qMakePair(seqno, isDirectRoute));
                if (privateChatDialogs.contains(origin)) {
                    qDebug() << "Updating open private chat window";
                    privateChatDialogs.value(origin)->updateDestinationIPandPort(address, port);
                }
                return true;
            }
        }
    }
    else {
        qDebug() << "Updating origin information for: " << origin;
        qDebug() << address;
        originMap.insert(origin, qMakePair(address, port));
        originDirectIndirectMap.insert(origin, qMakePair(seqno, isDirectRoute));
        if (privateChatDialogs.contains(origin)) {
            qDebug() << "Updating open private chat window";
            privateChatDialogs.value(origin)->updateDestinationIPandPort(address, port);
        }
        return true;
    }
    
    return false;
}

void PrivateMessagingPanel::buttonClicked(QString destinationName) {
    qDebug() << "Starting private chat with " + destinationName;
    PrivateChatDialog *privateChat = new PrivateChatDialog(hostName, destinationName, socket, &originMap);
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


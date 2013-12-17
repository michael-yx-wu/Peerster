#include "PrivateMessagingPanel.hh"
#include <qmath.h>

PrivateMessagingPanel::PrivateMessagingPanel(QString hostname) {
    hostName = hostname;
    originBox = new QGroupBox("Private Messaging");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
    buttonMapper = new QSignalMapper(this);
    privateChatMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this,
            SLOT(buttonClicked(QString)));
    connect(privateChatMapper, SIGNAL(mapped(QString)), this,
            SLOT(windowClosed(QString)));
    
    //Encryption
    p = 88951;
    g = 5;
    dlGroup = QCA::DLGroup(p, g);
    y = rand() % 53;
    QCA::BigInteger pubKeyInt = 1;
    for (QCA::BigInteger i = 0; i < y; i+=1) {
        pubKeyInt *= g;
    }
    pubKeyInt%=p;
    pubKey = pubKeyInt.toString();
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
    // Create a new button for previously unknown origins
    if (!originMap.contains(origin)) {
        QPushButton *originButton = new QPushButton(origin);
        buttonMapper->setMapping(originButton, originButton->text());
        connect(originButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
        originList->addWidget(originButton);
    }
    
    // If last route to origin was direct route, update only if new route is
    // direct route or if new route has higher seqno
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

// Open new chat dialog window on button click
void PrivateMessagingPanel::buttonClicked(QString destinationName) {
    qDebug() << "Starting private chat with " + destinationName;
    
    // only open if i've calculated symmetric key with this destinationName
    if(!keyMap.contains(destinationName))
    {
        DHKeyMessage message(hostName, destinationName, Constants::HOPLIMIT, pubKey);
        sendDHKeyMessage(message);
        return;
    }
    
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

#pragma mark - Process Private Messages

void PrivateMessagingPanel::processAudioMessage(QMap<QString, QVariant> datapacket) {
    QString origin = datapacket.value(Constants::xOrigin).toString();
    PrivateChatDialog *chatDialog;
    if (!privateChatDialogs.contains(origin)) {
        buttonClicked(origin);
    }
    chatDialog = privateChatDialogs.value(origin);
    chatDialog->getVoipPanel()->processAudioMessage(datapacket);
}

void PrivateMessagingPanel::processChatMessage(QMap<QString, QVariant> datapacket) {
    QString origin = datapacket.value(Constants::xOrigin).toString();
    PrivateChatDialog *chatDialog;
    if (!privateChatDialogs.contains(origin)) {
        buttonClicked(origin);
    }
    chatDialog = privateChatDialogs.value(origin);
    chatDialog->addTextToPrivateChat(datapacket.value(Constants::xChatText).toString());
}

void PrivateMessagingPanel::sendDHKeyMessage(DHKeyMessage message) {
    qDebug() << "Sending DHKeyMessage";
    QByteArray datagram = message.getSerializedMessage();
    QString dest = message.getDest();
    QHostAddress destIP = originMap.value(dest).first;
    quint16 destPort = originMap.value(dest).second;
    
    socket->writeDatagram(datagram.data(), datagram.size(), destIP, destPort);
}

void PrivateMessagingPanel::processDHKeyMessage(QMap<QString, QVariant> datapacket){
    QString origin = datapacket.value(Constants::xOrigin).toString();
    QString dest = datapacket.value(Constants::xDest).toString();
    QString key = datapacket.value(Constants::xDHKeyData).toString();
    
    QCA::BigInteger keyInt(key);
    QCA::BigInteger privKey = 1;
    for (QCA::BigInteger i = 0; i < y; i+=1) {
        privKey *= keyInt;
    }
    privKey%=p;
    
    QByteArray privBytes(privKey.toString().toAscii());
    QCA::SymmetricKey symKey(privBytes);
    
    if (!keyMap.contains(origin)) {
        // respond
        DHKeyMessage response(hostName, origin, Constants::HOPLIMIT, pubKey);
        sendDHKeyMessage(response);
    }

    //insert
    keyMap.insert(origin, symKey);    
}

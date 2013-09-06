#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QMap>
#include <sys/time.h>

#include "main.hh"

#pragma mark - Setup

// Initialize ChatDialog's private variables
ChatDialog::ChatDialog() {
    // Establish hostname as localhostname + pid
    hostname = QHostInfo::localHostName() + QString::number(rand()) + QString::number(rand());
//    qDebug() << QHostInfo::localDomainName();
    QHostInfo::lookupHost(QHostInfo::localHostName()+QHostInfo::localDomainName(), this, SLOT(myIPResults(QHostInfo)));
    sleep(5);
	
    // Create and add widgets to our ChatDialog
    setWindowTitle(hostname);
	textview = new QTextEdit(this);
	textview->setReadOnly(true);
	chatbox = new Textbox(this);
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(chatbox);
	setLayout(layout);
    chatbox->setFocus();
    
    // Initialize the socket and bind it to a UDP port
    minport = 32768 + (getuid() % 4096)*4;
    maxport = minport + 3;
    socket = new QUdpSocket(this);
    if (!bind()) {
        exit(1);
    }
    setWindowTitle(QString::number(myport));
    messageNo = 1;
    
    // Create timers
    mongerTimer = new QTimer(this);
    antiEntropyTimer = new QTimer(this);
    mongerTimer->setSingleShot(true);
//    antiEntropyTimer->start(5000);
    
	// Connect signals to their appropriate slots
    connect(chatbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(mongerTimer, SIGNAL(timeout()), this, SLOT(mongerTimeout()));
    connect(antiEntropyTimer, SIGNAL(timeout()), this, SLOT(antiEntropyTimeout()));

    // Add the ports in my port range to my peer list
//    for (int i = minport; i <= maxport; i++) {
//        if (i != myport) {
//            ChatDialog::updatePeerList(QHostAddress::LocalHost, i);
//        }
//    }
}

// Attempt to bind to a UDP port in range
bool ChatDialog::bind() {
    for (int p = minport; p <= maxport; p++) {
        if (socket->bind(myIP, p)) {
            qDebug() << "My IP:" << myIP;
            myport = p;
            return true;
        }
    }
    qDebug() << "My IP:" << myIP;    
    qDebug() << "Oops, no ports in my default range " << minport << "-" << maxport << " available";
	return false;
}

void ChatDialog::resolvePeer(QString hostPort) {
    qDebug() << "Starting resolving process";
    int indexOfColon = hostPort.indexOf(":");
    if (indexOfColon == -1) {
        qDebug() << "Not adding peer. Bad format: " << hostPort;
        return;
    }
    
    QString host = hostPort.left(indexOfColon);
    QHostAddress hostIP = QHostAddress(host);
    quint16 port = hostPort.mid(indexOfColon+1).toUInt();
    if (QAbstractSocket::IPv4Protocol == hostIP.protocol()) {
        updatePeerList(hostIP, port);
    }
    else {
        QEventLoop loop;
        connect(this, SIGNAL(lookupDone()), &loop, SLOT(quit()));
        QHostInfo::lookupHost(host, this, SLOT(lookupHostResults(QHostInfo)));
        loop.exec();
        while (loop.isRunning());
        updatePeerList(foundAddresses.front(), port);
        foundAddresses.clear();
    }
}

void ChatDialog::lookupHostResults(const QHostInfo &host) {
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed: " << host.errorString();
        return;
    }
    foreach (const QHostAddress &address, host.addresses()) {
        qDebug() << "Found address: " << address;
        foundAddresses.push_back(address);
    }
    emit ChatDialog::lookupDone();
}

void ChatDialog::myIPResults(const QHostInfo &host) {
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed: " << host.errorString();
        return;
    }
    foreach (const QHostAddress &address, host.addresses()) {
        qDebug() << "Found address: " << address;
        myIP.setAddress(address.toIPv4Address());
        return;
    }
}

void ChatDialog::updatePeerList(QHostAddress address, quint16 port) {
    std::vector<Peer>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++) {
        if ((*it).address == address && (*it).port == port) {
            return;
        }
    }
    qDebug() << "Adding : " << address << " port: " << port;
    peers.push_back(Peer(address, port));
}

# pragma mark - Process Incoming Datagrams

// Continue reading datagrams while the socket has pending datagrams
void ChatDialog::processPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        
        // Update peer list if necessary
        ChatDialog::updatePeerList(sender, senderPort);
        
        // Deserialize the datapacket
        QMap<QString, QVariant> datapacket;
        QDataStream stream(&datagram, QIODevice::ReadOnly);
        stream >> datapacket;
        
        // Check to see if datagram is a status or chat message
        if (datapacket.contains("Origin")) {
            if (processRumorMessage(datapacket, sender, senderPort)) {
                ChatDialog::sendStatusMessage(sender, senderPort);
                Peer p = peers.at(rand() % peers.size());
                Message message = Message(datapacket.value("Origin").toString(), datapacket.value("SeqNo").toUInt(), datapacket.value("ChatText").toString());
                rumorMonger(message, p.address, p.port);
            }
        }
        else {
            processStatusMessage(datapacket, sender, senderPort);
        }
    }
}

bool ChatDialog::processRumorMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort) {
    // Check to see if we have already seen this message
    QString origin = datapacket.value("Origin").toString();
    quint32 seqno = datapacket.value("SeqNo").toUInt();
    if (messages.hasMessage(origin, seqno)) {
        return false;
    }
    
    // We know about this origin and we have not seen this message,
    // but there is another message that should come before it
    if (status.contains(origin) && status.value(origin).toUInt() != seqno) {
        qDebug() << "I want message #: " << status.value(origin).toUInt() << "from " << origin << " I got message #: " << seqno;
        ChatDialog::sendStatusMessage(sender, senderPort);
        return false;
    }
    
    // Display the new message
    QString message = datapacket.value("ChatText").toString();
    textview->append(message);

    // Update status and save the message
    status[origin] = seqno+1;
    messages.addMessage(origin, seqno, message);

    return true;
}

void ChatDialog::processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort) {
    // Received a status message -- stop the timer
    mongerTimer->stop();
    qDebug() << "Stopping monger timer... ";
    QString origin = "";
    quint32 seqno = -1;
    QString chatText = "";
    Message message;
    bool mongerRumor = false, sendStatus = false;
    
    QMap<QString, QVariant> peerStatus = datapacket.value("Want").toMap();
    QMap<QString, QVariant>::iterator it;
    
    // Compare my status against the peer status
    for (it = status.begin(); it != status.end(); it++) {
        // Peer does not know about an origin
        if (!peerStatus.contains(it.key())) {
            origin = it.key();
            seqno = 1;
            chatText = messages.getMessage(origin, seqno);
            mongerRumor = true;
            break;
        }
        // Peer does not have a message that I have
        else if (status.value(it.key()).toUInt() > peerStatus.value(it.key()).toUInt()) {
            qDebug() << windowTitle()+" MyKey: " << it.key() << "Value: " << status.value(it.key());
            qDebug() << windowTitle()+" PeerKey: " << it.key() << "Value: " << peerStatus.value(it.key());
            origin = it.key();
            seqno = peerStatus.value(origin).toUInt();
            chatText = messages.getMessage(origin, seqno);
            mongerRumor = true;
            break;
        }
    }
    
    // Compare peer status against my status
    for (it = peerStatus.begin(); it != peerStatus.end(); it++) {
        // I do not know about an origin
        if (!status.contains(it.key())) {
            sendStatus = true;
            break;
        }
        // I do not have a message that peer does
        else if (peerStatus.value(it.key()).toUInt() > status.value(it.key()).toUInt()) {
            sendStatus = true;
            break;
        }
    }

    if (mongerRumor) {
        qDebug() << "Message requested: " << origin << " " << seqno << " " << chatText;
        message = Message(origin, seqno, chatText);
        rumorMonger(message, sender, senderPort);
    }
    else if (sendStatus) {
        qDebug() << "Requesting Messages from: " << sender << " Port: " << senderPort;
        ChatDialog::sendStatusMessage(sender, senderPort);
    }
    
}

void ChatDialog::sendChatMessage(Message message, QHostAddress address, quint16 port) {
    qDebug() << "Chat Message to: " + address.toString() + " Port: " + QString::number(port);
    QByteArray datagram = message.getSerializedMessage();
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

void ChatDialog::sendStatusMessage(QHostAddress address, quint16 port) {
    // Create the message
    QMap<QString, QVariant> statusMessage;
    statusMessage.insert("Want", status);
    
    // Serialize the message
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << statusMessage;
    
    // Send the message
    qDebug() << "Sending Status Message to: " + address.toString() + " Port: " + QString::number(port);
    
    QMap<QString, QVariant>::iterator it;
    for (it = status.begin(); it != status.end(); it++) {
        qDebug() << it.key() << ": " << it.value();
    }
    
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

#pragma mark - Chat

// Send the current message to neighbors
void ChatDialog::gotReturnPressed() {
    Message message = Message(hostname, messageNo, chatbox->toPlainText());
    
    textview->append(message.getMessage());
    messages.addMessage(message.getOrigin(), message.getSeqno(), message.getMessage());
    status[hostname] = ++messageNo;
    chatbox->clear();
    
    // Rumor monger at a random peer
    if (peers.size() == 0) return;
    Peer p = peers.at(rand() % peers.size());
    rumorMonger(message, p.address, p.port);
}

#pragma mark - Rumor Mongering

void ChatDialog::rumorMonger(Message message, QHostAddress address, quint16 port) {
    ChatDialog::sendChatMessage(message, address, port);
    // Update the last sent messages to my peers
    lastSentMessages[address.toString()] = message;
    lastTarget = Peer(address, port);
    mongerTimer->start(2000);
}

void ChatDialog::mongerTimeout() {
    // If "heads", rumor monger again
    qDebug() << "Monger Timeout";
    if (rand() % 2 == 1) {
        qDebug() << "Trying to monger again";
        Message message = lastSentMessages.value(lastTarget.address.toString());
        rumorMonger(message, lastTarget.address, lastTarget.port);
    }
}

#pragma mark - Anti-entropy 

void ChatDialog::antiEntropyTimeout() {
    qDebug() << "Anti-Entropy timeout";
    if (peers.size() == 0) return;
    Peer p = peers.at(rand() % peers.size());
    ChatDialog::sendStatusMessage(p.address, p.port);
}

#pragma mark

int main(int argc, char **argv) {
	// Initialize Qt toolkit
	QApplication app(argc,argv);
    
	// Create an initial chat dialog window
	ChatDialog dialog;
    
    dialog.show();
    
    // Display hostname
    qDebug();
    qDebug() << "My Hostname: " + dialog.hostname;
    qDebug() << "My Port: " + QString::number(dialog.myport);
    
    // Attempt to parse host:port strings
    for (int i = 1; i < argc; i++) {
        dialog.resolvePeer(argv[i]);
    }
    
	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


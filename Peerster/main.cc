#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QTextCodec>
#include <QMap>
#include <time.h>

#include "main.hh"

#pragma mark - Setup

const QString ChatDialog::xOrigin = "Origin";
const QString ChatDialog::xSeqNo = "SeqNo";
const QString ChatDialog::xChatText = "ChatText";
const QString ChatDialog::xWant = "Want";
const QString ChatDialog::xDest = "Dest";
const QString ChatDialog::xHopLimit = "HopLimit";
const QString ChatDialog::xLastIP = "LastIP";
const QString ChatDialog::xLastPort = "LastPort";

// Initialize ChatDialog's private variables
ChatDialog::ChatDialog() {
    shouldForwardMessages = true;
    
    // Establish hostname as localhostname + pid
    hostname = QHostInfo::localHostName() + QString::number(rand()) + QString::number(rand());
    qDebug() << QHostInfo::localHostName()+"."+QHostInfo::localDomainName();
    
    QEventLoop loop;
    connect(this, SIGNAL(lookupDone()), &loop, SLOT(quit()));
    QHostInfo::lookupHost(QHostInfo::localHostName()+"."+QHostInfo::localDomainName(), this, SLOT(myIPResults(QHostInfo)));
    loop.exec();
    while (loop.isRunning());
	
    // Create and add widgets to our ChatDialog
    setWindowTitle(hostname);
	textview = new QTextEdit(this);
	textview->setReadOnly(true);
	chatbox = new Chatbox(this);
    addHostBox = new Chatbox(this);
    
	QGridLayout *layout = new QGridLayout();
	layout->addWidget(textview);
	layout->addWidget(chatbox);
    layout->addWidget(addHostBox);
    layout->addWidget(privateMessagingPanel.getOriginBox(),0 , 1);
	setLayout(layout);
    chatbox->setFocus();
    
    // Initialize the socket and bind it to a UDP port
    minport = 32768 + (getuid() % 4096)*4;
    maxport = minport + 3;
    socket = new QUdpSocket(this);
    if (!bind()) {
        exit(1);
    }
    
    privateMessagingPanel.setSocket(socket);
    
    messageNo = 1;
    
    // Create timers
    antiEntropyTimer = new QTimer(this);
    routingTimer = new QTimer(this);
    
	// Connect signals to their appropriate slots
    connect(chatbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressedChatBox()));
    connect(addHostBox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressedHostBox()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(antiEntropyTimer, SIGNAL(timeout()), this, SLOT(antiEntropyTimeout()));
    connect(routingTimer, SIGNAL(timeout()), this, SLOT(routeMonger()));
    
    // Start timers
    antiEntropyTimer->start(5000);
    routingTimer->start(60000);//change to once per minute
    QTimer::singleShot(1000, this, SLOT(routeMonger()));
    
    // Add the ports in my port range to my peer list
    //    for (int i = minport; i <= maxport; i++) {
    //        if (i != myport) {
    //            ChatDialog::updatePeerList(myIP, i);
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
    emit lookupDone();
}

void ChatDialog::myIPResults(const QHostInfo &host) {
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed: " << host.errorString();
        return;
    }
    foreach (const QHostAddress &address, host.addresses()) {
        qDebug() << "Found address: " << address;
        myIP.setAddress(address.toIPv4Address());
        emit lookupDone();
        break;
    }
}

void ChatDialog::updatePeerList(QHostAddress address, quint16 port) {
    if (address != myIP) {
        std::vector<Peer>::iterator it;
        for (it = peers.begin(); it != peers.end(); it++) {
            if ((*it).address == address && (*it).port == port) {
                return;
            }
        }
        qDebug() << "Adding : " << address << " port: " << port;
        peers.push_back(Peer(address, port));
    }
}

# pragma mark - Process Incoming Datagrams

// Continue reading datagrams while the socket has pending datagrams
void ChatDialog::processPendingDatagrams() {
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;
    Peer p;
    Message message;
    
    while (socket->hasPendingDatagrams()) {
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        
        // Update peer list if necessary
        updatePeerList(sender, senderPort);
        
        // Deserialize the datapacket
        QMap<QString, QVariant> datapacket;
        QDataStream stream(&datagram, QIODevice::ReadOnly);
        stream >> datapacket;
        
        // Check to see if datagram is a status or chat/route message
        if (datapacket.contains(xOrigin)) {
//            if (processRumorMessage(datapacket, sender, senderPort)) {
//                ChatDialog::sendStatusMessage(sender, senderPort);
//                
//                // Rumor monger if chat message
//                if (datapacket.contains(xChatText)) {
//                    p = peers.at(rand() % peers.size());
//                    message = Message(datapacket.value(xOrigin).toString(), datapacket.value(xSeqNo).toUInt(), datapacket.value(xChatText).toString(), sender.toIPv4Address(), senderPort);
//                    rumorMonger(message, p.address, p.port);
//                }
//                
//                // If route message, send to all known peers
//                else {
//                    message = Message(datapacket.value(xOrigin).toString(), datapacket.value(xSeqNo).toUInt(), sender.toIPv4Address(), senderPort);
//                    for(std::vector<Peer>::iterator it = peers.begin(); it != peers.end(); ++it) {
//                        sendMessage(message, (*it).address, (*it).port);
//                    }
//                }
//            }
            processRumorMessage(datapacket, sender, senderPort);
        }
        // Check to see if datagram is a private chat message
        else if (datapacket.contains(xDest)) {
            processPrivateMessage(datapacket);
        }
        else {
            processStatusMessage(datapacket, sender, senderPort);
        }
    }
}

void ChatDialog::processRumorMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort) {
    QString origin, hostPort, message;
    quint32 seqno, lastIP;
    quint16 lastPort;
    bool isDirectRoute = true;
    bool routeUpdated = false;
    Message msg;
    Peer p;
    
    // Check to see if we have already seen this rumor message
    origin = datapacket.value(xOrigin).toString();
    seqno = datapacket.value(xSeqNo).toUInt();
    if (messages.hasMessage(origin, seqno)) {
        return;
    }
    
    // Resolve peer if lastIP/lastPort information present
    // Send route information to privateMessagingPanel
    if (origin != hostname) {
        if (datapacket.contains(xLastIP) && datapacket.contains(xLastPort)) {
            lastIP = datapacket.value(xLastIP).toUInt();
            lastPort = datapacket.value(xLastPort).toUInt();
            hostPort = QString::number(lastIP) + ":" + QString::number(lastPort);
            resolvePeer(hostPort);
            isDirectRoute = false;
            routeUpdated = updatePrivateMessagingPanel(origin, lastIP, lastPort, seqno, isDirectRoute);
        }
        else {
            routeUpdated = updatePrivateMessagingPanel(origin, sender, senderPort, seqno, isDirectRoute);
        }
    }
    
    // We know about this origin and we have not seen this message,
    // but there is another message that should come before it
    if (status.contains(origin) && status.value(origin).toUInt() != seqno) {
        qDebug() << "I want message #: " << status.value(origin).toUInt() << "from " << origin << " I got message #: " << seqno;
        ChatDialog::sendStatusMessage(sender, senderPort);
        return;
    }
    
    // If ChatText exists, display/save message and update seqno, monger
    // If route message, monger if route to origin updated
    if (datapacket.contains(xChatText)) {
        message = datapacket.value(xChatText).toString();
        textview->append(message);
        status[origin] = seqno+1;
        messages.addMessage(origin, seqno, message);
        
        msg = Message(datapacket.value(xOrigin).toString(), datapacket.value(xSeqNo).toUInt(), datapacket.value(xChatText).toString(), sender.toIPv4Address(), senderPort);
        rumorMonger(msg);
    }
    else {
        if (routeUpdated) {
            msg = Message(datapacket.value(xOrigin).toString(), datapacket.value(xSeqNo).toUInt(), sender.toIPv4Address(), senderPort);
            for(std::vector<Peer>::iterator it = peers.begin(); it != peers.end(); ++it) {
                sendMessage(msg, (*it).address, (*it).port);
            }
        }
    }

}

void ChatDialog::processPrivateMessage(QMap<QString, QVariant> datapacket) {
    qDebug() << "Got private message";
    qDebug() << hostname;
    QString dest = datapacket.value(xDest).toString();
    QString message = datapacket.value(xChatText).toString();
    quint32 hoplimit = datapacket.value(xHopLimit).toUInt();
    qDebug() << dest;
    qDebug() << message;
    qDebug() << hoplimit;
    
    // Display message if I am intended targets
    if (QString::compare(dest, hostname) == 0) {
        textview->append(message);
    }
    
    // Forward message if -noforward not dependent
    else if (shouldForwardMessages) {
        if (--hoplimit == 0) {
            // Reached hoplimit. Stop forwarding private message
            return;
        }
        Message privateMessage = Message(dest, message, hoplimit);
        QByteArray datagram = privateMessage.getSerializedMessage();
        QMap<QString, QPair<QHostAddress, quint16> > originMap = privateMessagingPanel.getOriginMap();
        socket->writeDatagram(datagram.data(), datagram.size(), originMap.value(dest).first, originMap.value(dest).second);
    }
}

void ChatDialog::processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort) {
    QString origin;
    quint32 seqno;
    QString chatText;
    Message message;
    bool mongerRumor = false, sendStatus = false;
    
    QMap<QString, QVariant> peerStatus = datapacket.value(xWant).toMap();
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
        message = Message(origin, seqno, chatText, myIP.toIPv4Address(), myport);
        sendMessage(message, sender, senderPort);
    }
    else if (sendStatus) {
        ChatDialog::sendStatusMessage(sender, senderPort);
    }
    
}

void ChatDialog::sendMessage(Message message, QHostAddress address, quint16 port) {
    if (shouldForwardMessages == true || message.getMessage().isNull()) {
        if (message.getMessage().isNull()) {
            qDebug() << "Sending Route Message";
        }
        else {
            qDebug() << "Sending Chat Message";
        }
        QByteArray datagram = message.getSerializedMessage();
        socket->writeDatagram(datagram.data(), datagram.size(), address, port);
    }
    else {
        qDebug() << "NOT FORWARDING MESSAGE from " + message.getOrigin();
    }
}

void ChatDialog::sendStatusMessage(QHostAddress address, quint16 port) {
    // Create the message
    QMap<QString, QVariant> statusMessage;
    statusMessage.insert(xWant, status);
    
    // Serialize the message
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << statusMessage;
    
    // Send the message
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

#pragma mark - Chat

// Send the current message to neighbors
void ChatDialog::gotReturnPressedChatBox() {
    Message message = Message(hostname, messageNo, chatbox->toPlainText());
    textview->append(message.getMessage());
    messages.addMessage(message.getOrigin(), message.getSeqno(), message.getMessage());
    status[hostname] = ++messageNo;
    chatbox->clear();
    
    // Rumor monger at a random peer
    rumorMonger(message);
}

void ChatDialog::gotReturnPressedHostBox() {
    resolvePeer(addHostBox->toPlainText());
    addHostBox->clear();
}

#pragma mark - Rumor Mongering

// Monger message at a random peer
void ChatDialog::rumorMonger(Message message) {
    if (peers.size() == 0) return;
    Peer peer = peers.at(rand() % peers.size());
    rumorMonger(message, peer.address, peer.port);
}

// Monger message at a specified peer
void ChatDialog::rumorMonger(Message message, QHostAddress address, quint16 port) {
    ChatDialog::sendMessage(message, address, port);
    newMessages.enqueue(message);
    QTimer::singleShot(2500, this, SLOT(mongerTimeout()));
}

void ChatDialog::mongerTimeout() {
    // If "heads", rumor monger again
    if (rand() % 2) {
        qDebug() << "Trying to monger again";
        if (!newMessages.isEmpty()) {
            if (peers.size() == 0) return;
            Peer p = peers.at(rand() % peers.size());
            rumorMonger(newMessages.dequeue(), p.address, p.port);
        }
    }
}

#pragma mark - Anti-entropy

void ChatDialog::antiEntropyTimeout() {
    if (peers.size() == 0) return;
    Peer p = peers.at(rand() % peers.size());
    ChatDialog::sendStatusMessage(p.address, p.port);
}

#pragma mark - Routing

// Monger route message to all peers
void ChatDialog::routeMonger() {
    qDebug() << "Route Mongering!";
    Message message = Message(hostname, messageNo);
    
    for(std::vector<Peer>::iterator it = peers.begin(); it != peers.end(); ++it) {
        sendMessage(message, (*it).address, (*it).port);
    }
}

bool ChatDialog::updatePrivateMessagingPanel(QString origin, QHostAddress address, quint16 port, quint32 seqno, bool isDirectRoute) {
    return privateMessagingPanel.updateOrigins(origin, address, port, seqno, isDirectRoute);
}

#pragma mark

int main(int argc, char **argv) {
    srand(time(NULL));
    
	// Initialize Qt toolkit
	QApplication app(argc,argv);
    
	// Create an initial chat dialog window
	ChatDialog dialog;
    dialog.show();
    dialog.routeMonger();
    
    // Display hostname
    qDebug();
    qDebug() << "My Hostname: " + dialog.hostname;
    qDebug() << "My Port: " + QString::number(dialog.myport);
    
    // Attempt to parse host:port strings
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-noforward") {
            dialog.shouldForwardMessages = false;
        }
        else {
            dialog.resolvePeer(argv[i]);
        }
    }
    
	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


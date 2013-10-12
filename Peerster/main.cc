#include <unistd.h>
#include <time.h>

#include "main.hh"

#pragma mark - Setup

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
    filePanel = new FilePanel(hostname, &peers);
    privateMessagingPanel = new PrivateMessagingPanel();
    
	QGridLayout *layout = new QGridLayout();
	layout->addWidget(textview);
	layout->addWidget(chatbox);
    layout->addWidget(addHostBox);
    layout->addWidget(privateMessagingPanel->getOriginBox(), 0, 1);
    layout->addWidget(filePanel->getGroupBox(), 1, 1);
    
	setLayout(layout);
    chatbox->setFocus();
    
    // Initialize the socket and bind it to a UDP port
    minport = 32768 + (getuid() % 4096)*4;
    maxport = minport + 3;
    socket = new QUdpSocket(this);
    if (!bind()) {
        exit(1);
    }
    
    updatePrivateMessagingPanel(hostname, myIP, myport, 1, true);
    
    filePanel->setSocket(socket);
    filePanel->setPrivateMessagingPanel(privateMessagingPanel);
    privateMessagingPanel->setSocket(socket);
    
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
    antiEntropyTimer->start(2000);
    routingTimer->start(10000);//change to once per minute
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
        
        // Process the datapacket
        if (datapacket.contains(Constants::xDest)) {
            processPrivateMessage(datapacket);
        }
        else if (datapacket.contains(Constants::xOrigin)) {
            processRumorMessage(datapacket, sender, senderPort);
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
    
    // If search request message pass to FilePanel and finish
    if (datapacket.contains(Constants::xSearchRequest)) {
        filePanel->handleSearchRequest(SearchRequestMessage(datapacket.value(Constants::xOrigin).toString(), datapacket.value(Constants::xSearchRequest).toString(), datapacket.value(Constants::xBudget).toUInt()));
        return;
    }
    
    // Check to see if we have already seen this rumor message
    origin = datapacket.value(Constants::xOrigin).toString();
    seqno = datapacket.value(Constants::xSeqNo).toUInt();
    if (messages.hasMessage(origin, seqno)) {
        return;
    }
    
    // Resolve peer if lastIP/lastPort information present
    // Send route information to privateMessagingPanel
    if (origin != hostname) {
        if (datapacket.contains(Constants::xLastIP) && datapacket.contains(Constants::xLastPort)) {
            lastIP = datapacket.value(Constants::xLastIP).toUInt();
            lastPort = datapacket.value(Constants::xLastPort).toUInt();
            hostPort = QHostAddress(lastIP).toString() + ":" + QString::number(lastPort);
            qDebug() << "Sending this to resolve peer method: " + hostPort;
            resolvePeer(hostPort);
            isDirectRoute = false;
            routeUpdated = updatePrivateMessagingPanel(origin, QHostAddress(lastIP), lastPort, seqno, isDirectRoute);
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
    if (datapacket.contains(Constants::xChatText)) {
        message = datapacket.value(Constants::xChatText).toString();
        textview->append(message);
        status[origin] = seqno+1;
        messages.addMessage(origin, seqno, message);
        
        msg = Message(origin, seqno, datapacket.value(Constants::xChatText).toString(), sender.toIPv4Address(), senderPort);
        rumorMonger(msg);
    }
    else {
        if (routeUpdated) {
            msg = Message(origin, seqno, sender.toIPv4Address(), senderPort);
            for(std::vector<Peer>::iterator it = peers.begin(); it != peers.end(); ++it) {
                sendMessage(msg, (*it).address, (*it).port);
            }
        }
    }

}

void ChatDialog::processPrivateMessage(QMap<QString, QVariant> datapacket) {
    Message privateMessage;
    QString origin, dest, message, searchReply;
    quint32 hoplimit;
    QByteArray blockRequest, blockReply, data;
    QVariantList matchNames, matchIDs;
    
    // Get data from the datapacket
    dest = datapacket.value(Constants::xDest).toString();
    hoplimit = datapacket.value(Constants::xHopLimit).toUInt()-1;
    if (datapacket.contains(Constants::xBlockReply)) {
        blockReply = datapacket.value(Constants::xBlockReply).toByteArray();
    }
    if (datapacket.contains(Constants::xBlockRequest)) {
        blockRequest = datapacket.value(Constants::xBlockRequest).toByteArray();
    }
    if (datapacket.contains(Constants::xSearchReply)) {
        searchReply = datapacket.value(Constants::xSearchReply).toString();
    }
    if (datapacket.contains(Constants::xMatchNames)) {
        matchNames = datapacket.value(Constants::xMatchNames).toList();
    }
    if (datapacket.contains(Constants::xMatchIDs)) {
        matchIDs = datapacket.value(Constants::xMatchIDs).toList();
    }
    if (datapacket.contains(Constants::xChatText)) {
        message = datapacket.value(Constants::xChatText).toString();
    }
    if (datapacket.contains(Constants::xData)) {
        data = datapacket.value(Constants::xData).toByteArray();
    }
    if (datapacket.contains(Constants::xHopLimit)) {
        hoplimit = datapacket.value(Constants::xHopLimit).toUInt();
        hoplimit--;
    }
    if (datapacket.contains(Constants::xOrigin)) {
        origin = datapacket.value(Constants::xOrigin).toString();
    }
    
    // Determine type of message to forward
    if (!blockReply.isEmpty()) {
        privateMessage = BlockReplyMessage(origin, dest, hoplimit, blockReply, data);
    }
    else if (!blockRequest.isEmpty()) {
        privateMessage = BlockRequestMessage(origin, dest, hoplimit, blockRequest);
    }
    else if (!searchReply.isEmpty()) {
        privateMessage = SearchReplyMessage(origin, dest, hoplimit, searchReply, matchNames, matchIDs);
    }
    else if (!message.isEmpty()) {
        privateMessage = Message(dest, message, hoplimit);
    }
    
    // I am the intended target of the private message
    if (dest == hostname) {
        if (datapacket.contains(Constants::xChatText)) {
            message = datapacket.value(Constants::xChatText).toString();
            textview->append(message);
        }
        // Process block reply
        else if (!blockReply.isEmpty()) {
            filePanel->handleBlockReply(privateMessage);
        }
        // Process block request
        else if (!blockRequest.isEmpty()) {
            filePanel->handleBlockRequest(privateMessage);
        }
        // Process search reply
        else if (!searchReply.isEmpty()) {
            filePanel->handleSearchReply(privateMessage);
        }
    }
    
    // Forward message if -noforward not specified and hopLimit not reached
    else if (shouldForwardMessages && hoplimit > 0) {
        QByteArray datagram = privateMessage.getSerializedMessage();
        QMap<QString, QPair<QHostAddress, quint16> > originMap = privateMessagingPanel->getOriginMap();
        QHostAddress targetIP = originMap.value(dest).first;
        quint16 targetPort = originMap.value(dest).second;
        socket->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
    }
}

void ChatDialog::processStatusMessage(QMap<QString, QVariant> datapacket, QHostAddress sender, quint16 senderPort) {
    QString origin;
    quint32 seqno = -1;
    QString chatText;
    Message message;
    bool mongerRumor = false, sendStatus = false;
    
    QMap<QString, QVariant> peerStatus = datapacket.value(Constants::xWant).toMap();
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
        message = Message(origin, seqno, chatText);
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
    statusMessage.insert(Constants::xWant, status);
    
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
    return privateMessagingPanel->updateOrigins(origin, address, port, seqno, isDirectRoute);
}

#pragma mark

int main(int argc, char **argv) {
    srand(time(NULL));
    
	// Initialize Qt toolkit
	QApplication app(argc,argv);
    QCA::init();
    Constants::setApplicationDirectory(QCoreApplication::applicationDirPath());
    
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


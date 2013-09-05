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
    hostname = QHostInfo::localHostName() + QString::number((quint32)getpid());
    
	setWindowTitle("Peerster");
    
	textview = new QTextEdit(this);
	textview->setReadOnly(true);
        
	textbox = new Textbox(this);
    
    // Set the height of the text box
    QFontMetrics m (textbox->font());
    int rowHeight = m.lineSpacing();
    textbox->setFixedHeight(3*rowHeight);
    
	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(textbox);
	setLayout(layout);
    
    // Initialize the socket and bind it to a UDP port
    minport = 32768 + (getuid() % 4096)*4;
    maxport = minport + 3;
    socket = new QUdpSocket(this);
    if (!bind()) {
        exit(1);
    }
    
    // Focus textbox when setup is done
    textbox->setFocus();
    
    status[hostname] = 1;
    
    // Create timers
    mongerTimer = new QTimer(this);
    
	// Connect signals to their appropriate slots
    connect(textbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(mongerTimer, SIGNAL(timeout()), this, SLOT(mongerTimeout()));

    // Get neighbors below and above -- this will change
    if (myport + 1 <= maxport) {
        ChatDialog::updatePeerList(QHostAddress::LocalHost, myport+1);
    }
    if (myport - 1 >= minport) {
        ChatDialog::updatePeerList(QHostAddress::LocalHost, myport-1);
    }
}

// Attempt to bind to a UDP port in range
bool ChatDialog::bind() {
    for (int p = minport; p <= maxport; p++) {
        if (socket->bind(QHostAddress::LocalHost, p)) {
            myport = p;
            return true;
        }
    }
    
    qDebug() << "Oops, no ports in my default range " << minport << "-" << maxport << " available";
	return false;
}

void ChatDialog::updatePeerList(QHostAddress address, quint16 port) {
    std::vector<Peer>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++) {
        if ((*it).address == address && (*it).port == port) {
            return;
        }
    }
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
            if (processRumorMessage(datapacket)) {
                ChatDialog::sendStatusMessage(sender, senderPort);
                rumorMonger(datagram, sender, senderPort);
            }
        }
        else {
            processStatusMessage(datapacket, sender, senderPort);
        }
    }
}

bool ChatDialog::processRumorMessage(QMap<QString, QVariant> datapacket) {
    // Check to see if we have already seen this message
    QString origin = datapacket.value("Origin").toString();
    quint32 seqno = datapacket.value("SeqNo").toUInt();
    if (messages.hasMessage(origin, seqno)) {
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
    
    QMap<QString, QVariant> peerStatus = datapacket.value("Want").toMap();
    QMap<QString, QVariant>::iterator it;
    for (it = peerStatus.begin(); it != peerStatus.end(); it++) {
        // If I have new messages that a peer has not seen - send one of those messages
        if (peerStatus.value(it.key()).toUInt() < status.value(it.key()).toUInt()) {
            QString origin = it.key();
            quint32 seqno = peerStatus.value(origin).toUInt();
            QString message = messages.getMessage(origin, seqno);
            QByteArray rumor = serializeMessage(message, origin, seqno);
            rumorMonger(rumor, sender, senderPort);
        }
        
        // If a peer has new messages that I have not seen - send status message to peer
        if (peerStatus.value(it.key()).toUInt() > status.value(it.key()).toUInt()) {
            qDebug() << "Requesting Messages from: " << sender << " Port: " << senderPort;
            ChatDialog::sendStatusMessage(sender, senderPort);
        }
    }
}

#pragma mark -

// Serialize rumor message into a QByteArray
QByteArray ChatDialog::serializeMessage(QString message, QString origin, quint32 seqno) {
    QMap<QString, QVariant> datapacket;
    datapacket.insert("ChatText", message);
    datapacket.insert("Origin", origin);
    datapacket.insert("SeqNo", seqno);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

// Send the current message to neighbors
void ChatDialog::gotReturnPressed() {
    QString message = textbox->toPlainText();
    
    // Serialize the message
    QByteArray datagram = ChatDialog::serializeMessage(message, hostname, status[hostname].toUInt());
    
    // Copy the message into my own chatbox
    // Update status and messages
    textview->append(message);
    messages.addMessage(hostname, status[hostname].toUInt(), message);
    status[hostname] = status[hostname].toUInt() + 1;

    // Clear textbox and increment message number
    textbox->clear();
    
    // Rumor monger at a random peer
    Peer p = peers.at(rand() % peers.size());
    rumorMonger(datagram, p.address, p.port);
}

#pragma mark - Rumor Mongering

void ChatDialog::sendChatMessage(QByteArray datagram, QHostAddress address, quint16 port) {
    qDebug() << "Chat Message to: " + address.toString() + " Port: " + QString::number(port);
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
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

void ChatDialog::rumorMonger(QByteArray datagram, QHostAddress peer, quint16 port) {
    ChatDialog::sendChatMessage(datagram, peer, port);
    mongerTimer->start(5000);
}

void ChatDialog::mongerTimeout() {
    // If "heads", rumor monger a 
    if (rand() % 2 == 1) {
        
    }
    
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

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


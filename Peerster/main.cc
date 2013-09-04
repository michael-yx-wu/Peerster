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
    
    // No messages sent yet!
    messageNo = 1;
    
    // Create timers
    mongerTimer = new QTimer(this);
    timeout = false;
    
	// Connect signals to their appropriate slots
    connect(textbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(mongerTimer, SIGNAL(timeout()), this, SLOT(mongerTimeout()));

    // Get neighbors below and above -- this will change
    if (myport + 1 <= maxport) {
        peers.push_back(Peer(QHostAddress::LocalHost, myport+1));
    }
    if (myport - 1 >= minport) {
        peers.push_back(Peer(QHostAddress::LocalHost, myport-1));
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

void ChatDialog::addPeer(QHostAddress address, quint32 p) {
    peers.push_back(Peer(address, p));
}

# pragma mark - Process Incoming Datagrams

// Continue reading datagrams while the socket has pending datagrams
void ChatDialog::processPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());
        
        // Deserialize the datapacket
        QMap<QString, QVariant> datapacket;
        QDataStream stream(&datagram, QIODevice::ReadOnly);
        stream >> datapacket;
        
        // Check to see if datagram is a status or chat message
        if (datapacket.contains("Origin")) {
            processRumorMessage(datapacket);
        }
        else {
            processStatusMessage(datapacket);
        }
    }
}

void ChatDialog::processRumorMessage(QMap<QString, QVariant> datapacket) {
    // Check to see if we have already seen this message
    QString origin = datapacket.value("Origin").toString();
    quint32 seqno = datapacket.value("SeqNo").toUInt();
    if (status.contains(origin)) {
        if (status.key(origin).toUInt() == seqno) {
            // We have already seen this message, do not display again
            return;
        }
    }
    
    // Display the new message
    QString message = datapacket.value("ChatText").toString();
    textview->append(message);

    // Save the message to peers3
    
    // Update status and messages maps
    status[origin] = seqno+1;
}

void ChatDialog::processStatusMessage(QMap<QString, QVariant> datapacket) {
//    QMap<QString, QVariant> unstatus = datapacket.value("Want").toMap();
//    QMap<QString, QVariant>::iterator it;
//    for (it = unstatus.begin(); it != unstatus.end(); it++) {
//        
//    }
    
    
    
    // Allow rumor mongering to proceed
    timeout = true;
}

#pragma mark -

// Serialize textbox text into a QByteArray
QByteArray ChatDialog::serializeMessage() {
    QVariant message = QVariant(textbox->toPlainText());
    QMap<QString, QVariant> datapacket;
    datapacket.insert("ChatText", message);
    datapacket.insert("Origin", hostname);
    datapacket.insert("SeqNo", messageNo);
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << datapacket;
    return datagram;
}

// Send the current message to neighbors
void ChatDialog::gotReturnPressed() {
    // Serialize the message
    QByteArray datagram = ChatDialog::serializeMessage();
    
    // Copy the message into my own chatbox and update status
    textview->append(textbox->toPlainText());
    status[hostname] = messageNo;

    // Clear textbox and increment message number
    messageNo++;
    textbox->clear();
    
    // RumorMonger!
    rumorMonger(datagram);
}

#pragma mark - Rumor Mongering 

void ChatDialog::sendChatMessage(QByteArray datagram, QHostAddress address, int port) {
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

void ChatDialog::sendStatusMessage(QHostAddress address, int port) {
    // Create the message
    QMap<QString, QVariant> statusMessage;
    QMap<QString, QVariant> wantMessages;
    QMap<QString, QVariant>::iterator it;
    for (it = status.begin(); it != status.end(); it++) {
        wantMessages.insert(it.key(), it.value().toUInt()+1);
    }
    statusMessage.insert("Want", wantMessages);
    
    // Serialize the message
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << statusMessage;
    
    // Send the message
    socket->writeDatagram(datagram.data(), datagram.size(), address, port);
}

void ChatDialog::rumorMonger(QByteArray datagram) {
    int size, selectRand, neighborPort;
    QHostAddress address;
    
    shouldContinueMongering = true;
    while (shouldContinueMongering) {
        size = peers.size();
        selectRand = rand() % size;
        address = peers.at(selectRand).address;
        neighborPort = peers.at(selectRand).port;
        ChatDialog::sendChatMessage(datagram, address, neighborPort);

        // Start the status message timer
        mongerTimer->start(5000);
        
        // Wait for status message or timeout
        while (!timeout);
        
        timeout = false;
    }
}

void ChatDialog::mongerTimeout() {
    shouldContinueMongering = (rand() % 2 == 1);
    
    // Allow rumor mongering to proceed
    timeout = true;
}

#pragma mark

int main(int argc, char **argv) {
	// Initialize Qt toolkit
	QApplication app(argc,argv);
    
	// Create an initial chat dialog window
	ChatDialog dialog;
    
    dialog.show();
    
    // Display hostname
    qDebug() << "My Hostname: " + dialog.hostname;
    qDebug() << "My Port: " + QString::number(dialog.myport);

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


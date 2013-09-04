#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QMap>
#include <sys/time.h>

#include "main.hh"

// Initialize ChatDialog's private variables
ChatDialog::ChatDialog() {
    // Establish hostname as localhostname + startup time in ms
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
    
    messageNo = 1;
    
	// Connect signals to their appropriate slots
    connect(textbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    
    // Focus textbox when setup is done
    textbox->setFocus();
}

// Attempt to bind to a UDP port in range
bool ChatDialog::bind() {
    for (int p = minport; p <= maxport; p++) {
        if (socket->bind(QHostAddress::LocalHost, p)) {
            return true;
        }
    }
    
    qDebug() << "Oops, no ports in my default range " << minport << "-" << maxport << " available";
	return false;
}

// Continue reading datagrams while the socket has pending datagrams
void ChatDialog::processPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());
        
        // Deserialize the message
        QMap<QString, QVariant> datapacket;
        QDataStream stream(&datagram, QIODevice::ReadOnly);
        stream >> datapacket;
        
        
        QString message = datapacket.value("ChatText").toString();
        QString origin = datapacket.value("Origin").toString();
        quint32 seqno = datapacket.value("SeqNo").toUInt();
        
        textview->append(message);
    }
}

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

void ChatDialog::gotReturnPressed() {
    // Serialize the message
    QByteArray datagram = ChatDialog::serializeMessage();

    // Send message to all peers
    for (int p = minport; p <= maxport; p++) {
        socket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::LocalHost, p);
    }
    
    // Clear textbox and increment message number
    messageNo++;
    textbox->clear();
}

int main(int argc, char **argv) {
	// Initialize Qt toolkit
	QApplication app(argc,argv);
    
	// Create an initial chat dialog window
	ChatDialog dialog;
    
    dialog.show();
    
    // Display hostname
    qDebug() << dialog.hostname;

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


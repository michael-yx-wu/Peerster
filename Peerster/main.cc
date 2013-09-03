#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <sys/time.h>
#include "main.hh"

ChatDialog::ChatDialog() {
    // Establish hostname as localhostname + startup time in ms
    timeval time;
    gettimeofday(&time, NULL);
    long msecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    hostname = QHostInfo::localHostName() + QString::number(msecs);
    
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

	// Register a callback on the textbox's returnPressed signal
	// so that we can send the message entered by the user.
	connect(textbox, SIGNAL(enterPressed()), this, SLOT(gotReturnPressed()));    
    
    // Focus textbox when setup is done
    textbox->setFocus();
}

void ChatDialog::gotReturnPressed() {
	// Echo the string locally.
	textview->append(textbox->toPlainText());

	// Clear the textbox to get ready for the next input message.
	textbox->clear();
    
    // Send the message to peers
    
    
    
}

NetSocket::NetSocket() {
    // Select 4 UDP ports
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;
}

bool NetSocket::bind() {
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			qDebug() << "bound to UDP port " << p;
			return true;
		}
	}

	qDebug() << "Oops, no ports in my default range " << myPortMin
		<< "-" << myPortMax << " available";
	return false;
}

void NetSocket::readPendingDatagrams() {
    
}

int main(int argc, char **argv) {
	// Initialize Qt toolkit
	QApplication app(argc,argv);

	// Create an initial chat dialog window
	ChatDialog dialog;
	dialog.show();

	// Create a UDP network socket
	NetSocket sock;
	if (!sock.bind())
		exit(1);
    
    
	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}


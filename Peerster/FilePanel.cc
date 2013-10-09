#include "FilePanel.hh"

const QString FilePanel::button1text = "Add Files(s)";
const QString FilePanel::button2text = "Download File";

FilePanel::FilePanel(QString someOrigin) {
    origin = someOrigin;
    isWaitingForMetafile = isWaitingForFile = false;
    
    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QGridLayout();
    fileShareBox->setLayout(fileShareBoxLayout);
    
    // Create signal mapper
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
    // Add files button
    selectFilesButton = new QPushButton(button1text);
    signalMapper->setMapping(selectFilesButton, button1text);
    connect(selectFilesButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    fileShareBoxLayout->addWidget(selectFilesButton, 0, 1);
    
    // Download file GUI
    targetNodeTextBox = new QLineEdit();
    hashTextBox = new QLineEdit();
    downloadFileButton = new QPushButton(button2text);
    signalMapper->setMapping(downloadFileButton, button2text);
    connect(downloadFileButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    targetNodeTextBox->setPlaceholderText("Target Node");
    hashTextBox->setPlaceholderText("Hash of Metafile");
    fileShareBoxLayout->addWidget(targetNodeTextBox, 1, 0);
    fileShareBoxLayout->addWidget(hashTextBox, 2, 0);
    fileShareBoxLayout->addWidget(downloadFileButton, 2, 1);
}

QGroupBox* FilePanel::getGroupBox() {
    return fileShareBox;
}

// Process button clicking
void FilePanel::buttonClicked(QString buttonName) {
    qDebug() << "Clicked";
    if (buttonName == button1text) {
        showDialog();
    }
    else if (buttonName == button2text) {
        QString targetNode = targetNodeTextBox->text();
        QByteArray hashTextBoxText;
        hashTextBoxText.append(hashTextBox->text());
        targetNodeTextBox->clear();
        hashTextBox->clear();
        sendBlockRequest(targetNode, hashTextBoxText);
    }
}

void FilePanel::handleBlockRequest(Message message) {
    QByteArray blockRequest = message.getBlockRequest();
    int i = 0;
    foreach(PeersterFile *f, files) {
        QByteArray blockRequest = message.getBlockRequest();
        if (blockRequest == f->getBlocklistHash()) {
            // send block reply with entire
            break;
        }
        else if ((i = f->getBlocklistMetafile().indexOf(blockRequest)) != -1) {
            // send block reply with corresponding block
            break;
        }
    }
}

// Send block request to the specified node
void FilePanel::sendBlockRequest(QString targetNode, QByteArray hash) {
    qDebug() << "Sending blockrequest to " + targetNode + "with hash" << hash;
    Message message = Message(origin, targetNode, Constants::HOPLIMIT, hash);
    QByteArray datagram = message.getSerializedMessage();
    QHostAddress targetIP = privateMessagingPanel->getOriginMap().value(targetNode).first;
    quint16 targetPort = privateMessagingPanel->getOriginMap().value(targetNode).second;
    socket->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// Send metafile to the specified node
void FilePanel::sendMetafile(QString targetNode, QByteArray hash, PeersterFile *f) {
    qDebug() << "Sending metafile to " + targetNode;
    Message message = Message(origin, targetNode, Constants::HOPLIMIT, hash, f->getBlocklistMetafile());
    QByteArray datagram = message.getSerializedMessage();
    QHostAddress targetIP = privateMessagingPanel->getOriginMap().value(targetNode).first;
    quint16 targetPort = privateMessagingPanel->getOriginMap().value(targetNode).second;
    socket->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// Show file selection dialog
void FilePanel::showDialog() {
    QStringList filesToAdd = QFileDialog::getOpenFileNames();
    for (int i = 0; i < filesToAdd.size(); i++) {
        qDebug() << "Scanning file: " + filesToAdd.at(i);
        files.append(new PeersterFile(filesToAdd.at(i)));
    }
}

void FilePanel::setPrivateMessagingPanel(PrivateMessagingPanel *somePanel) {
    privateMessagingPanel = somePanel;
}

void FilePanel::setSocket(QUdpSocket *parentSocket) {
    socket = parentSocket;
}


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

#pragma mark - GUI Actions

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
        if (!isWaitingForFile && !isWaitingForMetafile) {
            isWaitingForFile = isWaitingForMetafile = true;
            sendBlockRequest(targetNode, hashTextBoxText);
        }
        else {
            QMessageBox messageBox;
            messageBox.setText("Please wait. Busy downloading file.");
            messageBox.show();
        }
    }
}

// Show file selection dialog
void FilePanel::showDialog() {
    QStringList filesToAdd = QFileDialog::getOpenFileNames();
    for (int i = 0; i < filesToAdd.size(); i++) {
        qDebug() << "Scanning file: " + filesToAdd.at(i);
        files.append(new PeersterFile(filesToAdd.at(i)));
    }
}

#pragma mark - Handle Block Reply

void FilePanel::handleBlockReply(Message message) {
    QByteArray blockReply = message.getBlockReply();
    QByteArray data = message.getData();
    
    // Drop invalid block replies
    if (!validBlockReply(blockReply, data)) {
        return;
    }
    
    // Got a metafile -- start requesting data blocks
    if (isWaitingForMetafile && isWaitingForFile) {
        isWaitingForMetafile = false;
        blocksDownloaded = 0;
        numBlocksToDownload = data.size()/Constants::HASHSIZE;
        if (data.size() % Constants::HASHSIZE != 0) {
            numBlocksToDownload++;
        }
        metafileForPendingFile = message.getData();
        sendBlockRequest(message.getOrigin(), getMetaBlock(metafileForPendingFile, blocksDownloaded));
    }
    
    // Got block data
    else if (!isWaitingForMetafile && isWaitingForFile) {
        // Append to the file data
        blocksDownloaded++;
        dataForPendingFile.append(message.getData());
        
        if (blocksDownloaded == numBlocksToDownload) {
            // Stop downloading, add the file to our list of files
            isWaitingForFile = false;
            files.append(new PeersterFile(saveDownloadedFile(dataForPendingFile)));
            return;
        }
        
        // Send block request for next block in sequence
        sendBlockRequest(message.getOrigin(), getMetaBlock(metafileForPendingFile, blocksDownloaded));
    }
}

QByteArray FilePanel::getMetaBlock(QByteArray qbArray, int blockNumber) {
    return qbArray.mid(blockNumber*Constants::HASHSIZE, Constants::HASHSIZE);
}

QString FilePanel::saveDownloadedFile(QByteArray data) {
    QString filename = Constants::SAVE_DIRECTORY + QString::number(filesDownloaded++);
    QFile *f = new QFile(filename);
    f->open(QIODevice::WriteOnly);
    f->write(data);
    f->close();
    return filename;
}

bool FilePanel::validBlockReply(QByteArray hash, QByteArray block) {
    return QCA::Hash(Constants::HASHTYPE).hash(block).toByteArray() == hash;
}

#pragma mark - Handle Block Request

void FilePanel::handleBlockRequest(Message message) {
    QByteArray blockRequest = message.getBlockRequest();
    int i = 0;
    foreach(PeersterFile *f, files) {
        QByteArray blockRequest = message.getBlockRequest();
        // Send metafile
        if (blockRequest == f->getBlocklistHash()) {
            sendMetafileReply(message.getDest(), f, blockRequest);
            break;
        }
        // Send datablock
        else if ((i = f->getBlocklistMetafile().indexOf(blockRequest)) != -1) {
            int blockIndex = i/Constants::HASHSIZE;
            sendBlockReply(message.getDest(), f, blockRequest, blockIndex);
            break;
        }
    }
}

// Send block request to the specified node
void FilePanel::sendBlockRequest(QString targetNode, QByteArray hash) {
    qDebug() << "Sending blockrequest to " + targetNode + "with hash" << hash;
    Message message = Message(origin, targetNode, Constants::HOPLIMIT, hash);
    sendMessage(targetNode, message);
}

// Send metafile to the specified node
void FilePanel::sendMetafileReply(QString targetNode, PeersterFile *f, QByteArray hash) {
    qDebug() << "Sending metafile to " + targetNode;
    Message message = Message(origin, targetNode, Constants::HOPLIMIT, hash, f->getBlocklistMetafile());
    sendMessage(targetNode, message);
}

void FilePanel::sendBlockReply(QString targetNode, PeersterFile *f, QByteArray hash, int blockIndex) {
    qDebug() << "Sending block #" + QString::number(blockIndex) + " to targetNode";
    Message message = Message(origin, targetNode, Constants::HOPLIMIT, hash, f->getBlock(blockIndex));
    sendMessage(targetNode, message);
}

void FilePanel::sendMessage(QString targetNode, Message message) {
    QByteArray datagram = message.getSerializedMessage();
    QHostAddress targetIP = privateMessagingPanel->getOriginMap().value(targetNode).first;
    quint16 targetPort = privateMessagingPanel->getOriginMap().value(targetNode).second;
    socket->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

#pragma mark - Accessor Methods

QGroupBox* FilePanel::getGroupBox() {
    return fileShareBox;
}

void FilePanel::setPrivateMessagingPanel(PrivateMessagingPanel *somePanel) {
    privateMessagingPanel = somePanel;
}

void FilePanel::setSocket(QUdpSocket *parentSocket) {
    socket = parentSocket;
}


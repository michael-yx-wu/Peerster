#include "FilePanel.hh"

const QString FilePanel::button1text = "Add Files(s)";
const QString FilePanel::button2text = "Download File";
const QString FilePanel::button3text = "Search";

FilePanel::FilePanel(QString someOrigin, std::vector<Peer> *somePeers) {
    origin = someOrigin;
    isWaitingForMetafile = isWaitingForFile = false;
    filesDownloaded = 0;
    peers = somePeers;
    busyBox.setText("Busy downloading. Please try again later.");
    timeoutBox.setText("Download timeout. Perhaps file no longer exists.");

    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QGridLayout();
    fileShareBox->setLayout(fileShareBoxLayout);

    downloadTimeoutTimer = new QTimer(this);
    downloadTimeoutTimer->setSingleShot(true);
    connect(downloadTimeoutTimer, SIGNAL(timeout()), this, SLOT(requestTimeout()));
    
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
    targetNodeTextBox->setPlaceholderText("Target Node");
    hashTextBox = new QLineEdit();
    hashTextBox->setPlaceholderText("Hash of Metafile");
    downloadFileButton = new QPushButton(button2text);
    signalMapper->setMapping(downloadFileButton, button2text);
    connect(downloadFileButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    fileShareBoxLayout->addWidget(targetNodeTextBox, 1, 0);
    fileShareBoxLayout->addWidget(hashTextBox, 2, 0);
    fileShareBoxLayout->addWidget(downloadFileButton, 2, 1);
    
    // Search file GUI
    searchTextBox = new QLineEdit();
    searchTextBox->setPlaceholderText("Search Query");
    searchButton = new QPushButton(button3text);
    signalMapper->setMapping(searchButton, button3text);
    connect(searchButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    searchResults = new QListWidget();
    connect(searchResults, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(downloadFile(QListWidgetItem*)));
    fileShareBoxLayout->addWidget(searchTextBox, 3, 0);
    fileShareBoxLayout->addWidget(searchButton, 3, 1);
    fileShareBoxLayout->addWidget(searchResults, 4, 0);
}

#pragma mark - GUI Actions

// Process button clicking
void FilePanel::buttonClicked(QString buttonName) {
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
            filePanelBusy();
        }
    }
    else if (buttonName == button3text) {
        searchQuery = searchTextBox->text();
        searchTextBox->clear();
        sendSearchRequest(searchQuery, Constants::MAX_SEARCH_BUDGET);
    }
}

void FilePanel::downloadFile(QListWidgetItem *item) {
    qDebug() << "Downloading file: " + item->text();
    // Do not download file while busy
    if (isWaitingForMetafile || isWaitingForFile) {
        filePanelBusy();
        return;
    }
    isWaitingForMetafile = isWaitingForFile = true;
    const QString filename = item->text();
    QString targetNode = searchResultMap.value(filename)._origin;
    QByteArray metafileHash = searchResultMap.value(filename)._hash;
    sendBlockRequest(targetNode, metafileHash);
}

void FilePanel::filePanelBusy() {
    busyBox.show();
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
    qDebug() << "Got block reply";
    downloadTimeoutTimer->stop();
    QByteArray blockReply = message.getBlockReply();
    QByteArray data = message.getData();
    
    // Drop invalid block replies
    if (!validBlockReply(blockReply, data)) {
        qDebug() << "Invalid block";
        return;
    }
    
    // Got a metafile -- start requesting data blocks
    if (isWaitingForMetafile && isWaitingForFile) {
        qDebug() << "Got metafile";
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
    qDebug() << "Saving to Downloads folder";
    QString filename = Constants::SAVE_DIRECTORY + "/Downloads/file" + QString::number(filesDownloaded++);
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
    qDebug() << "Got block request";
    QByteArray blockRequest = message.getBlockRequest();
    int i = 0;
    foreach (PeersterFile *f, files) {
        QByteArray blockRequest = message.getBlockRequest();
        // Send metafile
        if (blockRequest == f->getBlocklistHash()) {
            sendMetafileReply(message.getOrigin(), f, blockRequest);
            break;
        }
        // Send datablock
        else if ((i = f->getBlocklistMetafile().indexOf(blockRequest)) != -1) {
            int blockIndex = i/Constants::HASHSIZE;
            sendBlockReply(message.getOrigin(), f, blockRequest, blockIndex);
            break;
        }
    }
}

void FilePanel::sendBlockRequest(QString targetNode, QByteArray hash) {
    qDebug() << "Sending blockrequest to " + targetNode + "with hash " << hash;
    Message message = BlockRequestMessage(origin, targetNode, Constants::HOPLIMIT, hash);
    sendMessage(targetNode, message);
    downloadTimeoutTimer->start(Constants::PACKET_TIMEOUT);
}

void FilePanel::sendMetafileReply(QString targetNode, PeersterFile *f, QByteArray hash) {
    qDebug() << "Sending metafile to " + targetNode;
    Message message = BlockReplyMessage(origin, targetNode, Constants::HOPLIMIT, hash, f->getBlocklistMetafile());
    sendMessage(targetNode, message);
}

void FilePanel::sendBlockReply(QString targetNode, PeersterFile *f, QByteArray hash, int blockIndex) {
    qDebug() << "Sending block #" + QString::number(blockIndex) + " to targetNode";
    Message message = BlockReplyMessage(origin, targetNode, Constants::HOPLIMIT, hash, f->getBlock(blockIndex));
    sendMessage(targetNode, message);
}

#pragma mark - Handle Search Reply

void FilePanel::handleSearchReply(Message message) {
    qDebug() << "Got search reply";
    downloadTimeoutTimer->stop();
    QVariantList filenames = message.getMatchNames();
    QVariantList metafileHashes = message.getMatchIDs();
    for (int i = 0; i < filenames.size(); i++) {
        QString filename = filenames.value(i).toString();
        QByteArray metafileHash = metafileHashes.value(i).toByteArray();
        searchResults->addItem(filename);
        searchResultMap.insert(filename, SearchResult(filename, message.getOrigin(), metafileHash));
    }
}

#pragma mark - Handle Search Request

void FilePanel::handleSearchRequest(Message message) {
    qDebug() << "Got search request";
    QString query = message.getSearchRequest();
    QVariantList filenames;
    QVariantList metafileHashes;
    
    // Local search
    foreach (PeersterFile *f, files) {
        if (f->getFilename().contains(query)) {
            filenames.append(f->getFilename());
            metafileHashes.append(f->getBlocklistHash());
        }
    }
    
    // Send search reply if we found something
    if (!filenames.isEmpty()) {
        sendSearchReply(message.getOrigin(), query, filenames, metafileHashes);
    }
    
    // Pass this request to some of our peers
    else {
        qDebug() << "Did not find " + query;
        // Spread the budget
        quint32 budget = message.getBudget()-1;
        QList<quint32> budgetSpread;
        for (quint32 i = 0; i < peers->size(); i++) {
            budgetSpread.append(0);
        }
        for (quint32 i = 0; i < budget; i++) {
            int index = i%peers->size();
            budgetSpread.insert(index, budgetSpread.value(index)+1);
        }
        
        // Forward search requests to peers
        while (!budgetSpread.isEmpty()) {
            quint32 newBudget = budgetSpread.front();
            budgetSpread.pop_front();
            if (newBudget != 0) {
                forwardSearchRequest(message.getOrigin(), query, newBudget);
            }
        }
    }
    
}

void FilePanel::sendSearchReply(QString targetNode, QString searchReply, QVariantList matchNames, QVariantList matchIDs) {
    qDebug() << "Send search reply";
    Message message = SearchReplyMessage(origin, targetNode, Constants::HOPLIMIT, searchReply, matchNames, matchIDs);
    sendMessage(targetNode, message);
}

void FilePanel::sendSearchRequest(QString query, quint32 budget) {
    qDebug() << "Sending search request";
    if (peers->size() == 0) return;
    Message message = SearchRequestMessage(origin, query, budget);
    Peer peer = peers->at(rand() % peers->size());
    sendMessage(peer, message);
}

void FilePanel::forwardSearchRequest(QString origin, QString query, quint32 budget) {
    qDebug() << "Forwarding search request";
    Message message = SearchRequestMessage(origin, query, budget);
    Peer peer = peers->at(rand() % peers->size());
    sendMessage(peer, message);
}

#pragma mark - Other File Download Methods

void FilePanel::requestTimeout() {
    isWaitingForFile = isWaitingForMetafile = false;
    timeoutBox.show();
}

void FilePanel::sendMessage(QString targetNode, Message message) {
    QByteArray datagram = message.getSerializedMessage();
    QHostAddress targetIP = privateMessagingPanel->getOriginMap().value(targetNode).first;
    quint16 targetPort = privateMessagingPanel->getOriginMap().value(targetNode).second;
    socket->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

void FilePanel::sendMessage(Peer peer, Message message) {
    QByteArray datagram = message.getSerializedMessage();
    socket->writeDatagram(datagram.data(), datagram.size(), peer.address, peer.port);
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


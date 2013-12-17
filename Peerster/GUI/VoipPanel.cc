#include "VoipPanel.hh"

const QString startVoIPButtonText = "Group VoIP Toggle";
const QString muteAllButtonText = "Mute All";
const QString startVoIPButtonPrivTxt = "VoIP Toggle";
const QString muteButtonText = "Mute";
const QString ON = "QPushButton { background-color: green; }";
const QString OFF = "QPushButton { background-color: red; }";
const int recordingTime = 1000;
const int delayThreshold = 5000;

#pragma mark - Group VoIP Panel

VoipPanel::VoipPanel(QString origin, QUdpSocket *socket,
                     std::vector<Peer> *peers,
                     QMap<QString, QVariant> *voipStatus) {
    hostname = origin;
    this->socket = socket;
    this->peers = peers;
    this->voipStatus = voipStatus;
    privChat = false;
    
    // VoIP Panel Setup
    buttonGroupBox = new QGroupBox("Group VoIP");
    buttonGroupList = new QVBoxLayout;
    buttonGroupBox->setLayout(buttonGroupList);
    
    // Initialize signal mapper
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this,
            SLOT(buttonClicked(QString)));
    
    // Connect VoIP button
    startVoIPButton = new QPushButton(startVoIPButtonText);
    startVoIPButton->setStyleSheet(OFF);
    buttonMapper->setMapping(startVoIPButton, startVoIPButton->text());
    connect(startVoIPButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(startVoIPButton);
    listening = false;
    
    // Connect mute all button
    muteAllButton = new QPushButton(muteAllButtonText);
    muteAllButton->setStyleSheet(ON);
    buttonMapper->setMapping(muteAllButton, muteAllButtonText);
    connect(muteAllButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(muteAllButton);
    muteAll = false;
    
    // Connect timer
    recordingTimer = new QTimer(this);
    connect(recordingTimer, SIGNAL(timeout()), this, SLOT(recordingTimeout()));
    
    // Format audio
    formatAudio();
}

#pragma mark - Private VoIP Panel

VoipPanel::VoipPanel(QString origin, QString destName, QUdpSocket *socket,
                     QMap<QString, QPair<QHostAddress, quint16> > *originMap,
                     QMap<QString, QCA::SymmetricKey> *keyMap) {
    hostname = origin;
    destinationName = destName;
    this->socket = socket;
    this->originMap = originMap;
    this->keyMap = keyMap;
    hopLimit = 10;
    privChat = true;
    
    // VoIP Panel Setup
    buttonGroupBox = new QGroupBox("Private VoIP");
    buttonGroupList = new QVBoxLayout;
    buttonGroupBox->setLayout(buttonGroupList);
    
    // Initialize signal mapper
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this,
            SLOT(buttonClicked(QString)));
    
    // Connect VoIP button
    startVoIPButton = new QPushButton(startVoIPButtonPrivTxt);
    startVoIPButton->setStyleSheet(OFF);
    buttonMapper->setMapping(startVoIPButton, startVoIPButton->text());
    connect(startVoIPButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(startVoIPButton);
    listening = false;
    
    // Connect mute all button
    muteAllButton = new QPushButton(muteButtonText);
    muteAllButton->setStyleSheet(ON);
    buttonMapper->setMapping(muteAllButton, muteButtonText);
    connect(muteAllButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(muteAllButton);
    muteAll = false;
    
    // Connect timer
    recordingTimer = new QTimer(this);
    connect(recordingTimer, SIGNAL(timeout()), this, SLOT(recordingTimeout()));
    
    // Format audio
    formatAudio();
}

void VoipPanel::updateDestinationIPandPort(QHostAddress destIP,
                                           quint16 destPort) {
    destinationIP = destIP;
    destinationPort = destPort;
}

#pragma mark - Accessor Methods

QGroupBox* VoipPanel::getButtonGroupBox() {
    return buttonGroupBox;
}

#pragma mark - GUI

void VoipPanel::buttonClicked(QString buttonName) {
    // Microphone toggle
    if (QString::compare(buttonName, startVoIPButtonText) == 0 ||
        QString::compare(buttonName, startVoIPButtonPrivTxt) == 0) {
        listening = !listening;
        if (listening) {
            qDebug() << "Voice Chat ON";
            recordingTimer->start(recordingTime);
            inputBuffers[currentBuffer].open(QIODevice::ReadWrite |
                                             QIODevice::Truncate);
            audioInput->start(&inputBuffers[currentBuffer]);
            recordingTimeout();
            startVoIPButton->setStyleSheet(ON);
        }
        else {
            qDebug() << "Voice Chat OFF";
            startVoIPButton->setStyleSheet(OFF);
        }
    } else if (QString::compare(buttonName, muteAllButtonText) == 0 ||
               QString::compare(buttonName, muteButtonText) == 0) {
        muteAll = !muteAll;
        if (muteAll) {
            qDebug() << "Mute All ON";
            muteAllButton->setStyleSheet(OFF);
        } else {
            qDebug() << "Mute All OFF";
            muteAllButton->setStyleSheet(ON);
        }
    }
}

# pragma mark - Audio Format

void VoipPanel::formatAudio() {
    format.setSampleRate(2000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/none");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    
    deviceInfo = QAudioDeviceInfo::defaultInputDevice();
    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Format not supported. Trying nearest format.";
        format = deviceInfo.nearestFormat(format);
    }
    
    audioInput = new QAudioInput(format, this);
}

# pragma mark - Audio Input

void VoipPanel::recordingTimeout() {
    audioInput->stop();
    
    // Switch buffers and continue recording
    currentBuffer = !currentBuffer;
    otherBuffer = !otherBuffer;
    inputBuffers[currentBuffer].open(QIODevice::ReadWrite|QIODevice::Truncate);
    audioInput->start(&inputBuffers[currentBuffer]);
    
    // Send buffer data
    QByteArray data = inputBuffers[otherBuffer].data();
    if (privChat == false) {
        AudioMessage message = AudioMessage(hostname, QDateTime::currentDateTimeUtc(),
                                            data);
        sendAudioMessage(message);
    }
    
    else {
        AudioMessage message = AudioMessage(hostname, destinationName, hopLimit,
                                            QDateTime::currentDateTimeUtc(), data);
        sendAudioPrivMessage(message, destinationIP, destinationPort);
    }
    
    inputBuffers[otherBuffer].close();
    
    if (!listening) {
        // No longer listening -- stop input and timer
        recordingTimer->stop();
        audioInput->stop();
        inputBuffers[currentBuffer].close();
    }
}

void VoipPanel::sendAudioMessage(AudioMessage message) {
    // Save audio metadata to voipStatus
    QString origin = message.getOrigin();
    QDateTime timestamp = message.getTimestamp();
    QString key = origin + timestamp.toString();
    voipStatus->insert(key, QString());
    
    // Send to all immediate peers
    qDebug() << "Sending Audio Message";
    unsigned i;
    QByteArray datagram = message.getSerializedMessage();
    for (i = 0; i < peers->size(); i++) {
        socket->writeDatagram(datagram.data(), datagram.size(),
                              peers->at(i).address, peers->at(i).port);
    }
}

void VoipPanel::sendAudioPrivMessage(AudioMessage message, QHostAddress destIP, quint16 destPort) {    
    qDebug() << "Sending private message";
    QByteArray datagram = message.getSerializedMessage();
    QCA::InitializationVector iv = QCA::InitializationVector(16);
    QCA::Cipher cipher = QCA::Cipher(QString("aes128"), QCA::Cipher::CBC,
                                     QCA::Cipher::DefaultPadding, QCA::Encode,
                                     keyMap->value(message.getDest()), iv);
    QCA::SecureArray secureData = datagram;
    QCA::SecureArray encryptedData = cipher.process(secureData);
    socket->writeDatagram(encryptedData.data(), encryptedData.size(), destIP, destPort);
}

# pragma mark - Audio Output

void VoipPanel::processAudioMessage(QMap<QString, QVariant> dataPacket) {
    // Refactoring
    QString origin, dest, key;
    QDateTime timestamp;
    QByteArray audioData;
    quint32 hoplimit;
    QHostAddress targetIP;
    quint16 targetPort;
    
    origin = dataPacket.value(Constants::xOrigin).toString();
    timestamp = dataPacket.value(Constants::xTimestamp).toDateTime();
    audioData = dataPacket.value(Constants::xAudioData).toByteArray();
    key = origin + timestamp.toString();
    
    //Process private messages
    if (dataPacket.contains(Constants::xDest)) {
        dest = dataPacket.value(Constants::xDest).toString();
        hoplimit = dataPacket.value(Constants::xHopLimit).toUInt()-1;
        
        // I am the intended target of the private audio message
        if (dest == hostname) {
            if (!muteAll && acceptableDelay(timestamp)) {
                qDebug() << "Playing private audio message";
                QByteArray encryptedAUdio = dataPacket.value(Constants::xAudioData).toByteArray();
                QCA::InitializationVector iv = QCA::InitializationVector(16);
                QCA::Cipher cipher = QCA::Cipher(QString("aes128"), QCA::Cipher::CBC,
                                                 QCA::Cipher::DefaultPadding, QCA::Encode,
                                                 keyMap->value(origin), iv);
                audioData = cipher.process(encryptedAUdio).toByteArray();
                if (cipher.ok()) {
                    qDebug() << "decrypted";
                } else {
                    qDebug() << "decrypt failed";
                }
                playAudioMessage(audioData);
            } else {
                qDebug() << "Person muted -- not playing (or delay too long)";
            }
        }
        
        // Audio message not for me -- forward message to destination
        else if (hoplimit > 0) {
            targetIP = originMap->value(dest).first;
            targetPort = originMap->value(dest).second;
            sendAudioPrivMessage(AudioMessage(origin, dest, hoplimit-1, timestamp, audioData), targetIP, targetPort);
        }
    }
    
    // Process group audio messages
    else {
        // Key is of format origin + timestamp
        origin = dataPacket.value(Constants::xOrigin).toString();
        timestamp = dataPacket.value(Constants::xTimestamp).toDateTime();
        audioData = dataPacket.value(Constants::xAudioData).toByteArray();
        key = origin + timestamp.toString();
        
        // Consider playing audio message if we have not yet heard it
        if (!voipStatus->contains(key)) {
            voipStatus->insert(key, QString());
            
            // Play if mute is off and timestamp within accpetable delay threshold
            if (!muteAll && acceptableDelay(timestamp)) {
                qDebug() << "Playing audio message";
                playAudioMessage(audioData);
            } else {
                qDebug() << "Group chat muted: mongering, not playing";
            }
            
            // Monger the audio message to peers
            qDebug() << "Mongering audio message to peers";
            sendAudioMessage(AudioMessage(origin, timestamp, audioData));
        }
    }
}

void VoipPanel::playAudioMessage(QByteArray audioData) {
    // Write audio data to file
    QFile *audioFile = new QFile();
    audioFile->setFileName("./" + QString::number(rand()) +
                           QString::number(rand()));
    audioFile->open(QIODevice::WriteOnly);
    QDataStream out(audioFile);
    out << audioData;
    audioFile->close();
    
    // Play audio message and enqueue
    audioFile->open(QIODevice::ReadOnly);
    QAudioOutput *output = new QAudioOutput(format, this);
    outputs.enqueue(output);
    audioFiles.enqueue(audioFile);
    connect(output, SIGNAL(stateChanged(QAudio::State)), this,
            SLOT(dequeueOutput(QAudio::State)));
    audioFile->seek(44);
    output->start(audioFile);
}

// Determine if the given timestamp is within delayThreshold of
// current system time
bool VoipPanel::acceptableDelay(QDateTime timestamp) {
    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    if (currentDateTime.date() == timestamp.date()) { // Matching dates
        QTime timestampTime = timestamp.time();
        QTime currentTime = currentDateTime.time();
        if (timestampTime.msecsTo(currentTime) < delayThreshold) {
            return true;
        }
    }
    return false;
}

void VoipPanel::dequeueOutput(QAudio::State state) {
    // Dequeue audio clip when finished playing
    if (state == QAudio::IdleState) {
        qDebug() << "Audio dequeue";
        QAudioOutput *output = outputs.dequeue();
        QFile *audioFile = audioFiles.dequeue();
        output->stop();
        audioFile->close();
        delete output;
        delete audioFile;
    }
}

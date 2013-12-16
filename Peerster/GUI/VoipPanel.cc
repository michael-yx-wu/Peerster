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
                     QMap<QString, QPair<QHostAddress, quint16> > *originMap) {
    hostname = origin;
    destinationName = destName;
    this->socket = socket;
    this->originMap = originMap;
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
        AudioMessage message = AudioMessage(destinationName, hopLimit,
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
    QByteArray datagram = message.getSerializedMessage();
    socket->writeDatagram(datagram.data(), datagram.size(), destIP, destPort);
}

# pragma mark - Audio Output

void VoipPanel::processAudioMessage(QMap<QString, QVariant> dataPacket) {
    //Processing private messages
    if (dataPacket.contains(Constants::xDest)) {
        QString dest = dataPacket.value(Constants::xDest).toString();
        QDateTime timestamp = dataPacket.value(Constants::xTimestamp).toDateTime();
        QByteArray audioData = dataPacket.value(Constants::xAudioData).toByteArray();
        quint32 hoplimit = dataPacket.value(Constants::xHopLimit).toUInt()-1;
        
        // I am the intended target of the private message
        if (dest == hostname) {
            if (!muteAll && acceptableDelay(timestamp)) {
                qDebug() << "Playing audio message";
                
                // Write audio data to file
                QFile *audioFile = new QFile();
                audioFile->setFileName("./" + QString::number(rand()) +
                                       QString::number(rand()));
                audioFile->open(QIODevice::WriteOnly);
                QDataStream out(audioFile);
                out << audioData;
                audioFile->close();
                
                audioFile->open(QIODevice::ReadOnly);
                
                // Play audio message and enqueue
                QAudioOutput *output = new QAudioOutput(format, this);
                outputs.enqueue(output);
                audioFiles.enqueue(audioFile);
                connect(output, SIGNAL(stateChanged(QAudio::State)), this,
                        SLOT(dequeueOutput(QAudio::State)));
                output->start(audioFile);
            } else {
                qDebug() << "Person muted -- not playing";
            }
        }
        
        // Forward messagehed
        else if (hoplimit > 0) {
            QHostAddress targetIP = originMap->value(dest).first;
            quint16 targetPort = originMap->value(dest).second;
            sendAudioPrivMessage(AudioMessage(dest, hoplimit, timestamp, audioData), targetIP, targetPort);
        }
        return;
    }
    
    // Key is of format origin + timestamp
    QString origin = dataPacket.value(Constants::xOrigin).toString();
    QDateTime timestamp = dataPacket.value(Constants::xTimestamp).toDateTime();
    QByteArray audioData = dataPacket.value(Constants::xAudioData).toByteArray();
    QString key = origin + timestamp.toString();
    
    // Consider playing audio message if we have not yet heard it
    if (!voipStatus->contains(key)) {
        voipStatus->insert(key, QString());
        
        // Play if mute is off and timestamp within accpetable delay threshold
        if (!muteAll && acceptableDelay(timestamp)) {
            qDebug() << "Playing audio message";
            
            // Write audio data to file
            QFile *audioFile = new QFile();
            audioFile->setFileName("./" + QString::number(rand()) +
                                   QString::number(rand()));
            audioFile->open(QIODevice::WriteOnly);
            QDataStream out(audioFile);
            out << audioData;
            audioFile->close();
            
            audioFile->open(QIODevice::ReadOnly);
            
            // Play audio message and enqueue
            QAudioOutput *output = new QAudioOutput(format, this);
            outputs.enqueue(output);
            audioFiles.enqueue(audioFile);
            connect(output, SIGNAL(stateChanged(QAudio::State)), this,
                    SLOT(dequeueOutput(QAudio::State)));
            audioFile->seek(44);
            output->start(audioFile);
        } else {
            qDebug() << "Group chat muted: mongering, not playing";
        }
        
        // Monger the audio message to peers
        qDebug() << "Mongering audio message to peers";
        sendAudioMessage(AudioMessage(origin, timestamp, audioData));
    }
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

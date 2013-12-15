#include "VoipPanel.hh"

const QString startVoIPButtonText = "Group VoIP Toggle";
const QString muteAllButtonText = "Mute All";
const QString ON = "QPushButton { background-color: green; }";
const QString OFF = "QPushButton { background-color: red; }";
const int recordingTime = 1000;

VoipPanel::VoipPanel(QString origin, QUdpSocket *socket, std::vector<Peer> *peers, QMap<QString, QVariant> *voipStatus) {
    this->origin = origin;
    this->socket = socket;
    this->peers = peers;
    this->voipStatus = voipStatus;
    
    // VoIP Panel Setup
    buttonGroupBox = new QGroupBox("VoIP");
    buttonGroupList = new QVBoxLayout;
    buttonGroupBox->setLayout(buttonGroupList);
    
    // Initialize signal mapper
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
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

#pragma mark - Accessor Methods

QGroupBox* VoipPanel::getButtonGroupBox() {
    return buttonGroupBox;
}

#pragma mark - GUI

void VoipPanel::buttonClicked(QString buttonName) {
    // Micropphone toggle
    if (QString::compare(buttonName, startVoIPButtonText) == 0) {
        listening = !listening;
        if (listening) {
            qDebug() << "Voice Chat ON";
            recordingTimer->start(recordingTime);
            inputBuffers[currentBuffer].open(QIODevice::ReadWrite|QIODevice::Truncate);
            audioInput->start(&inputBuffers[currentBuffer]);
//            buffer.open(QIODevice::WriteOnly|QIODevice::Truncate);
//            audioInput->start(&buffer);
            recordingTimeout();
            startVoIPButton->setStyleSheet(ON);
        }
        else {
            qDebug() << "Voice Chat OFF";
            startVoIPButton->setStyleSheet(OFF);
        }
    } else if (QString::compare(buttonName, muteAllButtonText) == 0) {
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
    AudioMessage message = AudioMessage(origin, QDateTime::currentDateTimeUtc(), data);
    sendAudioMessage(message);
    inputBuffers[otherBuffer].close();
    
    if (!listening) {
        // No longer listening -- stop input and timer
        recordingTimer->stop();
        audioInput->stop();
        inputBuffers[currentBuffer].close();
    }
}

void VoipPanel::sendAudioMessage(AudioMessage message) {
    // Send to all immediate peers
    qDebug() << "Sending Audio Message";
    unsigned i;
    QByteArray datagram = message.getSerializedMessage();
    for (i = 0; i < peers->size(); i++) {
        socket->writeDatagram(datagram.data(), datagram.size(), peers->at(i).address, peers->at(i).port);
    }
}

# pragma mark - Audio Output

void VoipPanel::processAudioMessage(QMap<QString, QVariant> dataPacket) {
    // Key is of format origin + timestamp
    QString origin = dataPacket.value(Constants::xOrigin).toString();
    QDateTime timestamp = dataPacket.value(Constants::xTimestamp).toDateTime();
    QByteArray audioData = dataPacket.value(Constants::xAudioData).toByteArray();
    QString key = origin + timestamp.toString();
    
    // Consider playing audio message if we have not yet heard it
    if (!voipStatus->contains(key)) {
        voipStatus->insert(key, QString());
        if (!muteAll) {
            qDebug() << "Playing audio message";
            
            // Write audio data to file
            QFile *audioFile = new QFile();
            audioFile->setFileName("./" + QString::number(rand()) + QString::number(rand()));
            audioFile->open(QIODevice::WriteOnly);
            QDataStream out(audioFile);
            out << audioData;
            audioFile->close();
            
            audioFile->open(QIODevice::ReadOnly);
            
            // Play audio message and enqueue
            QAudioOutput *output = new QAudioOutput(format, this);
            outputs.enqueue(output);
            audioFiles.enqueue(audioFile);
            connect(output, SIGNAL(stateChanged(QAudio::State)), this, SLOT(dequeueOutput(QAudio::State)));
            output->start(audioFile);
        } else {
            qDebug() << "Group chat muted: mongering, not playing";
        }
        
        // Monger the audio message to peers
        sendAudioMessage(AudioMessage(origin, timestamp, audioData));
    }
}

void VoipPanel::dequeueOutput(QAudio::State state) {
    // Dequeue audio clip and data regardless of stopped or suspended
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
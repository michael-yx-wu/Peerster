#include "VoipPanel.hh"

const QString startVoIPButtonText = "Group VoIP Toggle";
const int recordingTime = 1000;

VoipPanel::VoipPanel(QString origin, QUdpSocket *socket, std::vector<Peer> *peers) {
    this->origin = origin;
    this->socket = socket;
    this->peers = peers;
    
    // VoIP Panel Setup
    buttonGroupBox = new QGroupBox("VoIP");
    buttonGroupList = new QVBoxLayout;
    buttonGroupBox->setLayout(buttonGroupList);
    
    // Initialize signal mapper
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
    // Initialize and connect VoIP button
    startVoIPButton = new QPushButton(startVoIPButtonText);
    buttonMapper->setMapping(startVoIPButton, startVoIPButton->text());
    connect(startVoIPButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(startVoIPButton);
    
    // VoIP is off by default
    listening = false;
    
    // Connect timer
    recordingTimer = new QTimer(this);
    connect(recordingTimer, SIGNAL(timeout()), this, SLOT(recordingTimeout()));
    
    // Format audio
    formatAudio();
}

void VoipPanel::formatAudio() {
    format.setSampleRate(800);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    
    deviceInfo = QAudioDeviceInfo::defaultInputDevice();
    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Format not supported. Trying nearest format.";
        format = deviceInfo.nearestFormat(format);
    }
    
    audioInput = new QAudioInput(format, this);
}

void VoipPanel::buttonClicked(QString buttonName) {
    if (QString::compare(buttonName, startVoIPButtonText) == 0) {
        listening = !listening;
        if (listening) {
            qDebug() << "Voice Chat ON";
            recordingTimer->start(1000);
            recordingTimeout();
        }
        else {
            qDebug() << "Voice Chat OFF";
        }
    }
}

# pragma mark - Audio Input

void VoipPanel::recordingTimeout() {
    audioInput->stop();
    if (buffer.size() != 0) {
        // send buffer data
        AudioMessage message = AudioMessage(origin, QDateTime::currentDateTimeUtc(), buffer.data());
        sendAudioMessage(message);
        buffer.close();
    }
    
    if (!listening) {
        recordingTimer->stop();
    } else {
        // Start recording into buffer
        buffer.open(QIODevice::WriteOnly|QIODevice::Truncate);
        audioInput->start(&buffer);
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

void VoipPanel::playAudioMessage(QByteArray audioData) {
    qDebug() << "Playing audio message";
    
    QFile *audioFile = new QFile(audioData);
//    audioFile->open(QIODevice::ReadOnly);
    QBuffer *buffer = new QBuffer(&audioData);
    buffer->open(QIODevice::ReadOnly);
    
    // Play audio message and enqueue
    QAudioOutput *output = new QAudioOutput(format, this);
    outputs.enqueue(output);
    buffers.enqueue(buffer);
//    buffers.enqueue(audioFile);
    connect(output, SIGNAL(stateChanged(QAudio::State)), this, SLOT(dequeueOutput(QAudio::State)));
    output->start(audioFile);
    qDebug() << "Successful audio start";
}

void VoipPanel::dequeueOutput(QAudio::State state) {
    // Dequeue audio clip and data regardless of stopped or suspended
    if (state == QAudio::IdleState) {
        qDebug() << "Audio dequeue";
        QAudioOutput *output = outputs.dequeue();
        QBuffer *buffer = buffers.dequeue();
//        QFile *buffer = buffers.dequeue();
        output->stop();
        buffer->close();
        delete output;
        delete buffer;
    }
}

#pragma mark - Accessor Methods

QGroupBox* VoipPanel::getButtonGroupBox() {
    return buttonGroupBox;
}




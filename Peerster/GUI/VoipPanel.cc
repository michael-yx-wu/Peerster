#include "VoipPanel.hh"

const QString startVoIPButtonText = "Group VoIP Toggle";
const int recordingTime = 1000;

VoipPanel::VoipPanel(QString origin) {
    this->origin = origin;
    
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
    format.setFrequency(8000);
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
            qDebug() << "Listening ON";
            recordingTimer->start(1000);
            recordingTimeout();
        }
        else {
            qDebug() << "Listening OFF";
        }
    }
}

void VoipPanel::recordingTimeout() {
    audioInput->stop();
    if (buffer.size() != 0) {
        
    }
    
    // do something with full buffer
    //
    
    // Start recording into buffer
    buffer.open(QIODevice::ReadWrite);
    audioInput->start(&buffer);
    
    
    if (!listening) {
        recordingTimer->stop();
    }
}

#pragma mark - Accessor Methods

QGroupBox* VoipPanel::getButtonGroupBox() {
    return buttonGroupBox;
}



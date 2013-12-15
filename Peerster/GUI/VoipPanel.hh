#ifndef __Peerster__VoipPanel__
#define __Peerster__VoipPanel__

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QFile>
#include <QTimer>

#include <QButtonGroup>
#include <QGroupBox>
#include <QDebug>
#include <QPushButton>
#include <QSignalMapper>
#include <QString>
#include <QQueue>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <stdio.h>

#include "Peer.hh"
#include "../Messages/AudioMessage.hh"

class VoipPanel : public QObject {
    Q_OBJECT
    
public:
    VoipPanel(QString someOrigin, QUdpSocket *socket, std::vector<Peer> *peers);
    
    QGroupBox* getButtonGroupBox();
    
    void playAudioMessage(QByteArray audioData);
    
    public slots:
    
    void buttonClicked(QString buttonName);
    void dequeueOutput(QAudio::State state);
    void recordingTimeout();    
    
private:
    
    // VoIP Panel Format
    QGroupBox *buttonGroupBox;
    QVBoxLayout *buttonGroupList;
    
    // Panel buttons
    QPushButton *startVoIPButton;
    QPushButton *muteAllButton;
    QSignalMapper *buttonMapper;
    
    // File recording
    QAudioInput *audioInput;
    QBuffer buffer;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    QTimer *recordingTimer;
    void formatAudio();
    
    // Other mongering data
    QString origin;
    std::vector<Peer> *peers;
    
    // Audio Messaging
    QUdpSocket *socket;
    void sendAudioMessage(AudioMessage message);
    
    // Audio playback
    QQueue<QFile*> audioFiles;
    QQueue<QAudioOutput*> outputs;
    bool listening;
    bool muteAll;
};

#endif


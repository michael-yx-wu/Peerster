#ifndef __Peerster__VoipPanel__
#define __Peerster__VoipPanel__

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QTimer>

#include <QButtonGroup>
#include <QGroupBox>
#include <QDebug>
#include <QPushButton>
#include <QSignalMapper>
#include <QString>
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
    
    public slots:
    void buttonClicked(QString buttonName);
    void recordingTimeout();

private:
    bool listening;
    
    QString origin;
    
    QGroupBox* buttonGroupBox;
    QVBoxLayout* buttonGroupList;
    
    QPushButton* startVoIPButton;
    QSignalMapper* buttonMapper;
    
    // File recording
    QAudioInput *audioInput;
    QBuffer buffer;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    QTimer *recordingTimer;
    void formatAudio();
    
    // Audio Messaging
    QUdpSocket *socket;
    std::vector<Peer> *peers;
    void sendAudioMessage(AudioMessage message);
    
};

#endif


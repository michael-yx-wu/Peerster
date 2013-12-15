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
    VoipPanel(QString someOrigin, QUdpSocket *socket, std::vector<Peer> *peers,
              QMap<QString, QVariant> *voipStatus);

    VoipPanel(QString destName, QUdpSocket *socket);
    
    QGroupBox* getButtonGroupBox();
    
    void processAudioMessage(QMap<QString, QVariant> dataPacket);
    
    void updateDestinationIPandPort(QHostAddress destIP, quint16 destPort);

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
    bool listening;
    QAudioInput *audioInput;
    QBuffer inputBuffers[2];
    bool currentBuffer = false;
    bool otherBuffer = true;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    QTimer *recordingTimer;
    void formatAudio();
    
    // Other mongering data
    QString origin;
    std::vector<Peer> *peers;
    QMap<QString, QVariant> *voipStatus;
    
    // Audio Messaging
    QUdpSocket *socket;
    void sendAudioMessage(AudioMessage message);
    
    // Audio playback
    QQueue<QFile*> audioFiles;
    QQueue<QAudioOutput*> outputs;
    bool muteAll;
    
    bool acceptableDelay(QDateTime timestamp);

    //Private chat
    bool privChat = false;

    QHostAddress destinationIP;
    QString destinationName;
    quint16 destinationPort;
    quint32 hopLimit;
};

#endif


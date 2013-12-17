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
    // Group VoIP Panel Constructor
    VoipPanel(QString someOrigin, QUdpSocket *socket, std::vector<Peer> *peers,
              QMap<QString, QVariant> *voipStatus);

    // Private VoIP Panel Constructor
    VoipPanel(QString someOrigin, QString destName, QUdpSocket *socket,
              QMap<QString, QPair<QHostAddress, quint16> > *originMap);
    
    QGroupBox* getButtonGroupBox();
    
    // Private VoIP Target IP/Port update
    void updateDestinationIPandPort(QHostAddress destIP, quint16 destPort);

    // Process Audio Message
    void processAudioMessage(QMap<QString, QVariant> dataPacket);
    
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
    bool currentBuffer;
    bool otherBuffer;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    QTimer *recordingTimer;
    void formatAudio();
    
    // Other mongering data
    QString hostname;
    std::vector<Peer> *peers;
    QMap<QString, QVariant> *voipStatus;
    
    // Audio Messaging
    QUdpSocket *socket;
    void sendAudioMessage(AudioMessage message);
    void sendAudioPrivMessage(AudioMessage message, QHostAddress destIP,
                              quint16 destPort);

    
    // Audio playback
    QQueue<QFile*> audioFiles;
    QQueue<QAudioOutput*> outputs;
    void playAudioMessage(QByteArray audioData);
    bool muteAll;
    
    // Calculate if delay acceptable
    bool acceptableDelay(QDateTime timestamp);

    // Private chat metadata 
    bool privChat;
    QHostAddress destinationIP;
    QString destinationName;
    quint16 destinationPort;
    quint32 hopLimit;
    QMap<QString, QPair<QHostAddress, quint16> > *originMap;
};

#endif


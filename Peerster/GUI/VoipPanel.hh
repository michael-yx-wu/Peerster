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
#include <QVBoxLayout>
#include <stdio.h>

class VoipPanel : public QObject {
    Q_OBJECT
    
public:
    VoipPanel(QString someOrigin);
    
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
    
    QTimer *recordingTimer;
    
    // File recording
    QBuffer buffer;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    
    void formatAudio();
};

#endif


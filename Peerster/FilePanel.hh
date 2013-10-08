#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QList>
#include <QPushButton>
#include <QSignalMapper>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUdpSocket>

#include "Message.hh"
#include "PeersterFile.hh"
#include "PrivateMessagingPanel.hh"

class FilePanel : public QObject {
    Q_OBJECT
    
public:
    FilePanel(QString someOrigin);
    QGroupBox* getGroupBox();
    void setPrivateMessagingPanel(PrivateMessagingPanel *somePanel);
    void setSocket(QUdpSocket *parentSocket);
    
    public slots:
    void buttonClicked(QString buttonName);
    void handleBlockRequest(Message message);
    
private:
    static const QString button1text;
    static const QString button2text;
    static const QString button3text;
    QPushButton *downloadFileButton;
    QGroupBox *fileShareBox;
    QGridLayout *fileShareBoxLayout;
    QFileDialog *fileDialog;
    QList<PeersterFile*> files;
    quint32 hoplimit;
    QString origin;
    PrivateMessagingPanel *privateMessagingPanel;
    QSignalMapper *signalMapper;
    QPushButton *selectFilesButton;
    QUdpSocket *socket;
    QLineEdit *targetNodeTextBox;
    QLineEdit *hashTextBox;
    
    bool hasHash(QByteArray hash);
    void sendBlockRequest(QString targetode, QByteArray metafileHash);
    void showDialog();
};

#endif
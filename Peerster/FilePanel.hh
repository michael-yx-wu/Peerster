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
    bool isWaitingForMetafile;
    bool isWaitingForFile;
    QString origin;
    PrivateMessagingPanel *privateMessagingPanel;
    QSignalMapper *signalMapper;
    QPushButton *selectFilesButton;
    QUdpSocket *socket;
    QLineEdit *targetNodeTextBox;
    QLineEdit *hashTextBox;
    
    void sendBlockRequest(QString targetNode, QByteArray hash);
    void sendMetafile(QString targetNode, QByteArray hash, PeersterFile *f);
    void showDialog();
};

#endif
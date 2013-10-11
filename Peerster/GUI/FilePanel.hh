#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QtCrypto>

#include "../Messages/Message.hh"
#include "../Messages/BlockReplyMessage.hh"
#include "../Messages/BlockRequestMessage.hh"
#include "../Messages/SearchReplyMessage.hh"
#include "../Messages/SearchRequestMessage.hh"
#include "../FileSharing/PeersterFile.hh"
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
    void handleBlockReply(Message message);
    void handleBlockRequest(Message message);
    void sendBlockRequest(QString targetNode, QByteArray hash);
    
private:
    int blocksDownloaded;
    static const QString button1text;
    static const QString button2text;
    static const QString button3text;
    QByteArray dataForPendingFile;
    QPushButton *downloadFileButton;
    QList<PeersterFile*> files;
    int filesDownloaded;
    QGroupBox *fileShareBox;
    QGridLayout *fileShareBoxLayout;
    QFileDialog *fileDialog;
    bool isWaitingForFile;
    bool isWaitingForMetafile;
    int numBlocksToDownload;
    QString origin;
    QByteArray metafileForPendingFile;
    PrivateMessagingPanel *privateMessagingPanel;
    QSignalMapper *signalMapper;
    QPushButton *searchButton;
    QString searchQuery;
    QLineEdit *searchTextBox;
    QPushButton *selectFilesButton;
    QUdpSocket *socket;
    QLineEdit *targetNodeTextBox;
    QLineEdit *hashTextBox;
    
    QByteArray getMetaBlock(QByteArray qbArray, int blockNumber);
    QString saveDownloadedFile(QByteArray data);
    void sendBlockReply(QString targetNode, PeersterFile *f, QByteArray hash, int blockIndex);
    void sendMessage(QString targetNode, Message message);
    void sendMetafileReply(QString targetNode, PeersterFile *f, QByteArray hash);
    void showDialog();
    bool validBlockReply(QByteArray hash, QByteArray block);
};

#endif
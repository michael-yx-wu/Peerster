#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QtCrypto>

#include "PrivateMessagingPanel.hh"
#include "../Messages/Message.hh"
#include "../Messages/BlockReplyMessage.hh"
#include "../Messages/BlockRequestMessage.hh"
#include "../Messages/SearchReplyMessage.hh"
#include "../Messages/SearchRequestMessage.hh"
#include "../FileSharing/PeersterFile.hh"
#include "../FileSharing/SearchResult.hh"
#include "../Peer.hh"

class FilePanel : public QObject {
    Q_OBJECT
    
public:
    FilePanel(QString someOrigin, std::vector<Peer> *somePeers);
    QGroupBox* getGroupBox();
    
    void handleBlockReply(Message message);
    void handleBlockRequest(Message message);
    void handleSearchReply(Message message);
    void handleSearchRequest(Message message);
    void setPrivateMessagingPanel(PrivateMessagingPanel *somePanel);
    void setSocket(QUdpSocket *parentSocket);
    
    public slots:
    void buttonClicked(QString buttonName);
    void downloadFile(QListWidgetItem *item);
    
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
    QByteArray metafileForPendingFile;
    int numBlocksToDownload;
    QString origin;
    std::vector<Peer> *peers;
    PrivateMessagingPanel *privateMessagingPanel;
    QSignalMapper *signalMapper;
    QPushButton *searchButton;
    QString searchQuery;
    QListWidget *searchResults;
    QMap<QString, SearchResult> searchResultMap;
    QLineEdit *searchTextBox;
    QPushButton *selectFilesButton;
    QUdpSocket *socket;
    QLineEdit *targetNodeTextBox;
    QLineEdit *hashTextBox;
    
    QByteArray getMetaBlock(QByteArray qbArray, int blockNumber);
    QString saveDownloadedFile(QByteArray data);
    void sendBlockReply(QString targetNode, PeersterFile *f, QByteArray hash, int blockIndex);
    void sendBlockRequest(QString targetNode, QByteArray hash);
    void sendMessage(QString targetNode, Message message);
    void sendMessage(Peer peer, Message message);
    void sendMetafileReply(QString targetNode, PeersterFile *f, QByteArray hash);
    void sendSearchRequest(QString query, quint32 budget);
    void sendSearchReply(QString targetNode, QString searchReply, QVariantList matchNames, QVariantList matchIDs);
    void showDialog();
    bool validBlockReply(QByteArray hash, QByteArray block);
};

#endif
#ifndef __Peerster__SearchResultsWindow__
#define __Peerster__SearchResultsWindow__

#include <QByteArray>
#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QString>

#include "FilePanel.hh"
#include "../FileSharing/PeersterFile.hh"
#include "../Messages/Message.hh"
#include "../Messages/BlockReplyMessage.hh"
#include "../Messages/BlockRequestMessage.hh"
#include "../Messages/SearchReplyMessage.hh"
#include "../Messages/SearchRequestMessage.hh"

class SearchResultsWindow : public QDialog {
    Q_OBJECT
public:
    SearchResultsWindow(){};
    SearchResultsWindow(QString query, FilePanel *parentPanel);
    void addFile(QString origin, QString filename, QByteArray metafileHash);
    
    public slots:
    void downloadFile(QListWidgetItem *item);
    
private:
    QMap<QString, QByteArray> fileHashes;
    QMap<QString, QString> fileOrigins;
    QListWidget *filesList;
    QGridLayout *layout;
    FilePanel *parent;
    QSignalMapper *signalMapper;
    QString windowTitle;
    
};

#endif

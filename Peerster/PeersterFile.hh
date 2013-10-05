#ifndef __Peerster__File__
#define __Peerster__File__

#include <QtCrypto>
#include <QObject>
#include <QString>
#include <QFile>

class PeersterFile : public QObject {
    Q_OBJECT
    
public:
    PeersterFile();
    PeersterFile(const QString someFilename);
    QString getFilename();
    qint64 getFileSize();
    
private:
    QFile *file;
    QByteArray blockListHash;
    QFile *blockListMetafile;
    void scanFile();
};

#endif

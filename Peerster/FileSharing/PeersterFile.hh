#ifndef __Peerster__PeersterFile__
#define __Peerster__PeersterFile__

#include <QObject>
#include <QString>
#include <QFile>
#include <QtCrypto>

#include "../Constants.hh"

class PeersterFile : public QObject {
    Q_OBJECT
    
public:
    PeersterFile();
    PeersterFile(const QString someFilename);
    
    QByteArray getBlock(int blockIndex);
    QByteArray getBlocklistHash();
    QByteArray getBlocklistMetafile();
    QString getFilename();
    qint64 getFileSize();    
    void setBlocklistHash(QByteArray someHash);
    void setBlocklistMetafile(QByteArray someMetafile);
    
private:
    QFile *file;
    QByteArray blocklistHash;
    QByteArray blocklistMetafile;
    
    void scanFile();
};

#endif

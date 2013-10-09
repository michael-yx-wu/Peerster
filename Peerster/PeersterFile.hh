#ifndef __Peerster__File__
#define __Peerster__File__

#include <QtCrypto>
#include <QObject>
#include <QString>
#include <QFile>

#include "Constants.hh"

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

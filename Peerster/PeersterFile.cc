#include "PeersterFile.hh"

static qint64 BLOCKSIZE = 8192; // 8KB blocksize

PeersterFile::PeersterFile() {
    file = NULL;
}

PeersterFile::PeersterFile(const QString someFilename) {
    file = new QFile(someFilename);
}

QString PeersterFile::getFilename() {
    return file->fileName();
}

qint64 PeersterFile::getFileSize() {
    return file->size();
}

void scanFile(QFile *file) {
    QByteArray block;
    while ((block = file->read(BLOCKSIZE)).size() != 0) {
        
    }
}
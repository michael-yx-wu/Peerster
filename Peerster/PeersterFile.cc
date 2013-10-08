#include "PeersterFile.hh"

static qint64 BLOCKSIZE = 8192; // 8KB blocksize

PeersterFile::PeersterFile() {
    file = NULL;
}

PeersterFile::PeersterFile(const QString someFilename) {
    file = new QFile(someFilename);
    scanFile();
}

QString PeersterFile::getFilename() {
    return file->fileName();
}

qint64 PeersterFile::getFileSize() {
    return file->size();
}

void PeersterFile::scanFile() {
    QByteArray blockHashes;
    QByteArray block;
    while ((block = file->read(BLOCKSIZE)).size() != 0) {
        QCA::Hash hash("sha256");
        hash.update(block);
        blockHashes.append(hash.final().toByteArray());
    }
    QCA::Hash hash("sha256");
    hash.update(blockHashes);
    
    blocklistHash = hash.final().toByteArray();
    blocklistMetafile = blockHashes;
}

void PeersterFile::setBlocklistHash(QByteArray someHash) {
    blocklistHash = someHash;
}

void PeersterFile::setBlocklistMetafile(QByteArray someMetafile) {
    blocklistMetafile = someMetafile;
}
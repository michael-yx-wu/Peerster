#include "PeersterFile.hh"

PeersterFile::PeersterFile() {
    file = NULL;
}

PeersterFile::PeersterFile(const QString someFilename) {
    file = new QFile(someFilename);
    scanFile();
}



void PeersterFile::scanFile() {
    QByteArray blockHashes;
    QByteArray block;
    while ((block = file->read(Constants::BLOCKSIZE)).size() != 0) {
        QCA::Hash hash("sha256");
        hash.update(block);
        blockHashes.append(hash.final().toByteArray());
    }
    QCA::Hash hash("sha256");
    hash.update(blockHashes);
    
    blocklistHash = hash.final().toByteArray();
    blocklistMetafile = blockHashes;
}

#pragma mark - Accessor methods

QByteArray PeersterFile::getBlocklistHash() {
    return blocklistHash;
}

QByteArray PeersterFile::getBlocklistMetafile() {
    return blocklistMetafile;
}

QString PeersterFile::getFilename() {
    return file->fileName();
}

qint64 PeersterFile::getFileSize() {
    return file->size();
}

void PeersterFile::setBlocklistHash(QByteArray someHash) {
    blocklistHash = someHash;
}

void PeersterFile::setBlocklistMetafile(QByteArray someMetafile) {
    blocklistMetafile = someMetafile;
}
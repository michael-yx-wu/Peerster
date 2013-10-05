#include "PeersterFile.hh"

PeersterFile::PeersterFile() {
    filename = QString();
    filesize = 0;
}

PeersterFile::PeersterFile(const QString someFilename) {
    filename = someFilename;
    
}
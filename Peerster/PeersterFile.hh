#ifndef __Peerster__File__
#define __Peerster__File__

#include <QObject>
#include <QString>

class PeersterFile : public QObject {
    Q_OBJECT
    
public:
    PeersterFile();
    PeersterFile(const QString someFilename);
    
private:
    QString filename;
    quint32 filesize;
    
    
};

#endif

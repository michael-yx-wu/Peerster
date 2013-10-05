#ifndef __Peerster__File__
#define __Peerster__File__

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
    void scanFile(QFile *file);
};

#endif

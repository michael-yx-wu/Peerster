#ifndef __Peerster__SearchResult__
#define __Peerster__SearchResult__

#include <QString>

class SearchResult {
public:
    SearchResult() {};
    SearchResult(QString filename, QString origin, QByteArray hash);

    QString _filename;
    QString _origin;
    QByteArray _hash;
};

#endif

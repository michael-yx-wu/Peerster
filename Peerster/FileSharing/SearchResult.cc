#include "SearchResult.hh"

SearchResult::SearchResult(QString filename, QString origin, QByteArray hash) {
    _filename = filename;
    _origin = origin;
    _hash = hash;
}

const QString SearchResult::getFilename() {
    return _filename;
}

const QByteArray SearchResult::getHash() {
    return _hash;
}

const QString SearchResult::getOrigin() {
    return _origin;
}
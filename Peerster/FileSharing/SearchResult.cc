#include "SearchResult.hh"

SearchResult::SearchResult(QString filename, QString origin, QByteArray hash) {
    _filename = filename;
    _origin = origin;
    _hash = hash;
}

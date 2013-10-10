#ifndef __Peerster__Constants__
#define __Peerster__Constants__

#include <QCoreApplication>
#include <QString>

class Constants {
public:
    // QMap Keys
    static const QString xOrigin;
    static const QString xSeqNo;
    static const QString xChatText;
    static const QString xWant;
    static const QString xDest;
    static const QString xHopLimit;
    static const QString xLastIP;
    static const QString xLastPort;
    static const QString xBlockRequest;
    static const QString xBlockReply;
    static const QString xData;
    static const QString xSearch;
    static const QString xSearchReply;
    static const QString xBudget;
    static const QString xMatchNames;
    static const QString xMatchIDs; 
    
    // Application constants
    static const qint64 BLOCKSIZE;
    static const int HASHSIZE;
    static const QString HASHTYPE;
    static const quint32 HOPLIMIT;
    static const QString SAVE_DIRECTORY;
};


#endif

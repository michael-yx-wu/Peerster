#include "Constants.hh"

const QString Constants::xOrigin = "Origin";
const QString Constants::xSeqNo = "SeqNo";
const QString Constants::xChatText = "ChatText";
const QString Constants::xWant = "Want";
const QString Constants::xDest = "Dest";
const QString Constants::xHopLimit = "HopLimit";
const QString Constants::xLastIP = "LastIP";
const QString Constants::xLastPort = "LastPort";
const QString Constants::xBlockRequest = "BlockRequest";
const QString Constants::xBlockReply = "BlockReply";
const QString Constants::xData = "Data";
const QString Constants::xSearchRequest = "Search";
const QString Constants::xSearchReply = "SearchReply";
const QString Constants::xBudget = "Budget";
const QString Constants::xMatchNames = "MatchNames";
const QString Constants::xMatchIDs = "MatchIDs";

const qint64 Constants::BLOCKSIZE = 8192;
const int Constants::HASHSIZE = 32;
const QString Constants::HASHTYPE = "sha256";
const quint32 Constants::HOPLIMIT = 10;
const quint32 Constants::MAX_SEARCH_BUDGET = 10;
QString Constants::SAVE_DIRECTORY;

void Constants::setApplicationDirectory(QString path) {
    SAVE_DIRECTORY = path;
}
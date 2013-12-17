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
const QString Constants::xData = "FileData";
const QString Constants::xSearchRequest = "Search";
const QString Constants::xSearchReply = "SearchReply";
const QString Constants::xBudget = "Budget";
const QString Constants::xMatchNames = "MatchNames";
const QString Constants::xMatchIDs = "MatchIDs";
const QString Constants::xTimestamp = "Timestamp";
const QString Constants::xAudioData = "AudioData";
const QString Constants::xDHKeyData = "DHKey";

const qint64 Constants::BLOCKSIZE = 8192;
const int Constants::HASHSIZE = 32;
const QString Constants::HASHTYPE = "sha256";
const quint32 Constants::HOPLIMIT = 10;
QString Constants::SAVE_DIRECTORY;
const quint32 Constants::MAX_SEARCH_BUDGET = 10;
const int Constants::PACKET_TIMEOUT = 5000;

void Constants::setApplicationDirectory(QString path) {
    SAVE_DIRECTORY = path;
}
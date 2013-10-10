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

const qint64 Constants::BLOCKSIZE = 8192;
const int Constants::HASHSIZE = 32;
const QString Constants::HASHTYPE = "sha256";
const quint32 Constants::HOPLIMIT = 10;
const QString Constants::SAVE_DIRECTORY = QCoreApplication::applicationDirPath();
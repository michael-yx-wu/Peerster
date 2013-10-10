#include "BlockRequestMessage.hh"

BlockRequestMessage::BlockRequestMessage(const QString someOrigin, const QString someDest, const quint32 someHopLimit, const QByteArray someBlockRequest) {
    defaultBoolValues();
    blockRequestMessage = true;
    origin = someOrigin;
    dest = someDest;
    hopLimit = someHopLimit;
    blockRequest = someBlockRequest;
    
}

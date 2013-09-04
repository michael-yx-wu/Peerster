#include "Messages.hh"

void Messages::addMessage(QString origin, quint32 messageNo, QString message) {
        seenMessages[origin][messageNo] = message;
}
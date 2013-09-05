#include "Messages.hh"

void Messages::addMessage(QString origin, quint32 seqno, QString message) {
        seenMessages[origin][seqno] = message;
}

bool Messages::hasMessage(QString origin, quint32 seqno) {
    if (seenMessages.contains(origin)) {
        if (seenMessages.value(origin).contains(seqno)) {
            return true;
        }
    }
    return false;
}

QString Messages::getMessage(QString origin, quint32 seqno) {
    return seenMessages.value(origin).value(seqno);
}


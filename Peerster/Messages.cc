#include "Messages.hh"

void Messages::addMessage(QString origin, quint32 seqno, QString message) {
        seenMessages[origin][seqno] = message;
}

QString Messages::getMessage(QString origin, quint32 seqno) {
    return seenMessages.value(origin).value(seqno);
}
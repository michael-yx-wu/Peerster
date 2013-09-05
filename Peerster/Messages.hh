#ifndef __Peerster__Messages__
#define __Peerster__Messages__

#include <QMap>
#include <QString>
#include <QVariant>

class Messages {
private:
    QMap<QString, QMap<quint32, QString> > seenMessages;
    
public:
    Messages() {};
    void addMessage(QString origin, quint32 seqno, QString message);
    bool hasMessage(QString origin, quint32 seqno);
    QString getMessage(QString origin, quint32 seqno);
};

#endif /* defined(__Peerster__Messages__) */

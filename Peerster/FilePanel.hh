#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QList>
#include <QPushButton>
#include <QSignalMapper>
#include <QLineEdit>
#include <QVBoxLayout>

#include "PeersterFile.hh"

class FilePanel : public QObject {
    Q_OBJECT
    
public:
    FilePanel();
    QGroupBox* getGroupBox();
    void showDialog();

    public slots:
    void buttonClicked(QString buttonName);
    
private:
    static const QString button1text;
    static const QString button2text;
    static const QString button3text;
    QPushButton *downloadFileButton;
    QGroupBox *fileShareBox;
    QGridLayout *fileShareBoxLayout;
    QFileDialog *fileDialog;
    QList<PeersterFile*> files;
    QSignalMapper *signalMapper;
    QPushButton *selectFilesButton;
    QLineEdit *downloadFromNode;
    QLineEdit *metafileHash;
};

#endif
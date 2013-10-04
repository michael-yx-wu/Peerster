#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>


class FilePanel : public QObject {
    Q_OBJECT
    
public:
    FilePanel();
    QGroupBox* getGroupBox();
    
private:
//    QFileDialog *fileDialog;
    QSignalMapper *buttonMapper;
    QGroupBox *fileShareBox;
    QVBoxLayout *fileShareBoxLayout;
};

#endif
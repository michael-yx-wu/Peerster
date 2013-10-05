#ifndef __Peerster__FilePanel__
#define __Peerster__FilePanel__

#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>


class FilePanel : public QObject {
    Q_OBJECT
    
public:
    FilePanel();
    QGroupBox* getGroupBox();
    
    public slots:
    void showDialog();
    
private:
    QGroupBox *fileShareBox;
    QVBoxLayout *fileShareBoxLayout;
    QFileDialog *fileDialog;
};

#endif
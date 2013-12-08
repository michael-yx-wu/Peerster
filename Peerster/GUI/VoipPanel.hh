#ifndef __Peerster__VoipPanel__
#define __Peerster__VoipPanel__

#include <QButtonGroup>
#include <QGroupBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QString>
#include <QVBoxLayout>

class VoipPanel : public QObject {
    Q_OBJECT
    
public:
    VoipPanel(QString origin);
    
    QGroupBox* getButtonGroupBox();
    
    public slots:
    void buttonClicked(QString buttonName);

private:
    bool listening;
    
    QGroupBox* buttonGroupBox;
    QVBoxLayout* buttonGroupList;
    
    QPushButton* startVoIPButton;
    QString startVoIPButtonText = "Group VoIP Toggle";
    QSignalMapper* buttonMapper;
    
};

#endif


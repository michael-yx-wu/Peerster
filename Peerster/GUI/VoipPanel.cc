#include "VoipPanel.hh"

VoipPanel::VoipPanel(QString origin) {
    this->origin = origin;
    
    // VoIP Panel Setup
    buttonGroupBox = new QGroupBox("VoIP");
    buttonGroupList = new QVBoxLayout;
    buttonGroupBox->setLayout(buttonGroupList);
    
    // Initialize signal mapper
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
    // Initialize and connect VoIP button
    startVoIPButton = new QPushButton(startVoIPButtonText);
    buttonMapper->setMapping(startVoIPButton, startVoIPButton->text());
    connect(startVoIPButton, SIGNAL(clicked()), buttonMapper, SLOT(map()));
    buttonGroupList->addWidget(startVoIPButton);
    
    // VoIP is off by default
    listening = false;
}

QGroupBox* VoipPanel::getButtonGroupBox() {
    return buttonGroupBox;
}

void VoipPanel::buttonClicked(QString buttonName) {
    if (QString::compare(buttonName, startVoIPButtonText) == 0) {
        listening = !listening;
        if (listening) {
            qDebug() << "Listening ON";
        }
        else {
            qDebug() << "Listening OFF";
        }
    }
}

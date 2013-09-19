#include "PrivateMessagingPanel.hh"

PrivateMessagingPanel::PrivateMessagingPanel() {
    originBox = new QGroupBox("Known Origins");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
}

QGroupBox* PrivateMessagingPanel::getOriginBox() {
    return originBox;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port) {
    if (!origin.contains(origin)) {
        QPushButton *originButton = new QPushButton(origin);
        originList->addWidget(originButton);
        delete originButton;
    }
    originMap.insert(origin, qMakePair(address, port));
}

#include "PrivateMessagingPanel.hh"

PrivateMessagingPanel::PrivateMessagingPanel() {
    originBox = new QGroupBox("Known Origins");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(const QString)), this, SLOT(buttonClicked(const QString)));
}

QGroupBox* PrivateMessagingPanel::getOriginBox() {
    return originBox;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port) {
    qDebug() << "Updating origin information for: " << origin;
    if (!originMap.contains(origin)) {
        qDebug() << "New button for new origin!";
        QPushButton *originButton = new QPushButton(origin);
        originList->addWidget(originButton);
        connect(originButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(originButton, originButton->text());
    }
    originMap.insert(origin, qMakePair(address, port));
}

void PrivateMessagingPanel::buttonClicked(const QString text) {
    qDebug() << text << "clicked!!";
}
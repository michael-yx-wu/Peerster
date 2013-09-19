#include <QApplication>
#include "PrivateMessagingPanel.hh"


PrivateMessagingPanel::PrivateMessagingPanel() {
    originBox = new QGroupBox("Known Origins");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(buttonClicked(const QString &)));
}

QGroupBox* PrivateMessagingPanel::getOriginBox() {
    return originBox;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port) {
    qDebug() << "Updating origin information for: " << origin;
    if (!originMap.contains(origin)) {
        qDebug() << "New button for new origin!";
        QPushButton *originButton = new QPushButton(origin);
        connect(originButton, SIGNAL(released()), signalMapper, SLOT(map()));
        signalMapper->setMapping(originButton, originButton->text());
        originList->addWidget(originButton);
    }
    originMap.insert(origin, qMakePair(address, port));
}

void PrivateMessagingPanel::buttonClicked(const QString &text) {
    qDebug() << text << "clicked!!";
}
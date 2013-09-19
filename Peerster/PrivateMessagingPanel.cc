#include "PrivateMessagingPanel.hh"

PrivateMessagingPanel::PrivateMessagingPanel() {
    originBox = new QGroupBox("Known Origins");
    originList = new QVBoxLayout;
    originBox->setLayout(originList);
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
}

QGroupBox* PrivateMessagingPanel::getOriginBox() {
    return originBox;
}

void PrivateMessagingPanel::updateOrigins(QString origin, QHostAddress address, quint16 port) {
    qDebug() << "Updating origin information for: " << origin;
    if (!originMap.contains(origin)) {
        qDebug() << "New button for new origin!";
        QPushButton *originButton = new QPushButton(origin);
        signalMapper->setMapping(originButton, originButton->text());
        connect(originButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        originList->addWidget(originButton);
    }
    originMap.insert(origin, qMakePair(address, port));
}

void PrivateMessagingPanel::buttonClicked(QString text) {
    qDebug() << "Starting private chat with " + text;
    PrivateChatDialog *privateChat = new PrivateChatDialog(text);
    privateChatDialogs.insert(text, privateChat);
    privateChat->show();
}
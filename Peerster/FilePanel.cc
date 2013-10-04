#include "FilePanel.hh"

FilePanel::FilePanel() {
    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QVBoxLayout;
    fileShareBoxLayout->addWidget(new QPushButton("Add File(s)"));
    fileShareBox->setLayout(fileShareBoxLayout);
    
    buttonMapper = new QSignalMapper(this);
    connect(buttonMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
}

QGroupBox* FilePanel::getGroupBox() {
    return fileShareBox;
}
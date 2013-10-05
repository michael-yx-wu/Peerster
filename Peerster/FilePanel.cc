#include "FilePanel.hh"

const QString FilePanel::button1text = "Add Files(s)";
const QString FilePanel::button2text = "Download File";

FilePanel::FilePanel() {
    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QVBoxLayout;
    fileShareBox->setLayout(fileShareBoxLayout);
    
    // Create signal mapper
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
    // Add files button
    selectFilesButton = new QPushButton(button1text);
    fileShareBoxLayout->addWidget(selectFilesButton);
    signalMapper->setMapping(selectFilesButton, button1text);
    connect(selectFilesButton, SIGNAL(clicked()), signalMapper, SLOT(Map()));
    
    // Download file button
    downloadFileButton = new QPushButton(button2text);
    fileShareBoxLayout->addWidget(downloadFileButton);
    signalMapper->setMapping(downloadFileButton, button2text);
    connect(downloadFileButton, SIGNAL(clicked()), signalMapper, SLOT(Map()));
}

QGroupBox* FilePanel::getGroupBox() {
    return fileShareBox;
}

void FilePanel::showDialog() {
    QStringList filesToAdd = QFileDialog::getOpenFileNames();
    for (int i = 0; i < filesToAdd.size(); i++) {
        qDebug() << "Scanning file: " + filesToAdd.at(i);
        files.append(new PeersterFile(filesToAdd.at(i)));
    }
}

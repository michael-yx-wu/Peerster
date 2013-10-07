#include "FilePanel.hh"

const QString FilePanel::button1text = "Add Files(s)";
const QString FilePanel::button2text = "Download File";

FilePanel::FilePanel() {
    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QGridLayout();
    fileShareBox->setLayout(fileShareBoxLayout);
    
    // Create signal mapper
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonClicked(QString)));
    
    // Add files button
    selectFilesButton = new QPushButton(button1text);
    signalMapper->setMapping(selectFilesButton, button1text);
    connect(selectFilesButton, SIGNAL(clicked()), signalMapper, SLOT(Map()));
    fileShareBoxLayout->addWidget(selectFilesButton, 0, 1);
    
    // Download file GUI
    downloadFileButton = new QPushButton(button2text);
    signalMapper->setMapping(downloadFileButton, button2text);
    connect(downloadFileButton, SIGNAL(clicked()), signalMapper, SLOT(Map()));
    downloadFromNode = new QLineEdit();
    metafileHash = new QLineEdit();
    downloadFromNode->setPlaceholderText("Target Node");
    metafileHash->setPlaceholderText("Hash of Metafile");
    fileShareBoxLayout->addWidget(downloadFileButton, 1, 1);
    fileShareBoxLayout->addWidget(downloadFromNode, 1, 0);
    fileShareBoxLayout->addWidget(metafileHash, 2, 0);
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

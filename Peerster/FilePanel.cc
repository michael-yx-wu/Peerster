#include "FilePanel.hh"

FilePanel::FilePanel() {
    fileShareBox = new QGroupBox("File Sharing");
    fileShareBoxLayout = new QVBoxLayout;
    QPushButton *selectFiles = new QPushButton("Add File(s)");
    connect(selectFiles, SIGNAL(pressed()), this, SLOT(showDialog()));
    fileShareBoxLayout->addWidget(selectFiles);
    fileShareBox->setLayout(fileShareBoxLayout);
    
}

QGroupBox* FilePanel::getGroupBox() {
    return fileShareBox;
}

void FilePanel::showDialog() {
   QStringList filesToAdd = QFileDialog::getOpenFileNames();
    for (int i = 0; i < filesToAdd.size(); i++) {
        qDebug() << "Scanning file: " + filesToAdd.at(i);
        
    }
}

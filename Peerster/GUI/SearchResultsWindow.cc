#include "SearchResultsWindow.hh"

SearchResultsWindow::SearchResultsWindow(QString query, FilePanel *parentPanel) {
    windowTitle = "Search Results: " + query;
    parent = parentPanel;
    
    filesList = new QListWidget();
    connect(filesList, SIGNAL(itemDoubleClicked(QListWidgetItem)), this, SLOT(downloadFile(QListWidgetItem)));
    
    layout = new QGridLayout();
    layout->addWidget(filesList);
    setLayout(layout);
}

void SearchResultsWindow::addFile(QString origin, QString filename, QByteArray metafileHash) {
    fileHashes.insert(filename, metafileHash);
    fileOrigins.insert(filename, origin);
    filesList->addItem(filename);
}

void SearchResultsWindow::downloadFile(QListWidgetItem *item) {
    QString targetNode = fileOrigins.value(item->text());
    QByteArray metafileHash = fileHashes.value(item->text());
    parent->sendBlockRequest(targetNode, metafileHash);
}
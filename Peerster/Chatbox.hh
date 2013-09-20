#ifndef __Peerster__Chatbox__
#define __Peerster__Chatbox__

#include <iostream>
#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>

class Chatbox : public QTextEdit {
    Q_OBJECT
    
private:
    bool shift;
    
public:
    Chatbox(QWidget *parent = 0) : QTextEdit(parent) {
        QFontMetrics m (font());
        int rowHeight = m.lineSpacing();
        setFixedHeight(3*rowHeight);
        shift = false;
    };
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
signals:
    void enterPressed();
};

#endif

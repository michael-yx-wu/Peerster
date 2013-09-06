#ifndef __Peerster__Textbox__
#define __Peerster__Textbox__

#include <iostream>
#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>

class Textbox : public QTextEdit {
    Q_OBJECT
    
private:
    bool shift;
    
public:
    Textbox(QWidget *parent = 0) : QTextEdit(parent) {
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

#endif /* defined(__Peerster__Textbox__) */

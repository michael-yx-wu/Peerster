//
//  Textbox.h
//  Peerster
//
//  Created by Michael Wu on 8/31/13.
//
//

#ifndef __Peerster__Textbox__
#define __Peerster__Textbox__

#include <iostream>
#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>

class Textbox : public QTextEdit {
    Q_OBJECT
    
private:
    bool shift = false;
    
public:
    Textbox(QWidget *parent = 0) : QTextEdit(parent) {};
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
signals:
    void enterPressed();
};

#endif /* defined(__Peerster__Textbox__) */

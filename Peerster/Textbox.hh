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

class Textbox : public QTextEdit {
    Q_OBJECT
    
public:
    Textbox(QWidget *parent = 0) : QTextEdit(parent) {};
Q_SIGNALS:
    void returnPressed();
    void keyPressEvent(QKeyEvent *event);
};

#endif /* defined(__Peerster__Textbox__) */

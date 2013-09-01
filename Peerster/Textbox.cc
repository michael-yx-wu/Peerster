//
//  Textbox.cpp
//  Peerster
//
//  Created by Michael Wu on 8/31/13.
//
//

#include "Textbox.hh"

void Textbox::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift ) {
        Textbox::shift = true;
    }
    else if(event->key() == Qt::Key_Return && Textbox::shift == false) {
        emit enterPressed();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

void Textbox::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        Textbox::shift = false;
    }
    QTextEdit::keyReleaseEvent(event);
}
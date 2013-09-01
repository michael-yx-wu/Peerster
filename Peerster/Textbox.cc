//
//  Textbox.cpp
//  Peerster
//
//  Created by Michael Wu on 8/31/13.
//
//

#include "Textbox.hh"

void Textbox::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Return) {
        emit enterPressed();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}
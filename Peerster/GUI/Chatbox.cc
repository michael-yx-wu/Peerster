#include "Chatbox.hh"

void Chatbox::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift ) {
        Chatbox::shift = true;
    }
    if(event->key() == Qt::Key_Return && Chatbox::shift == false) {
        emit enterPressed();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

void Chatbox::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        Chatbox::shift = false;
    }
    QTextEdit::keyReleaseEvent(event);
}
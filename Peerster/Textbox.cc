#include "Textbox.hh"

void Textbox::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift ) {
        Textbox::shift = true;
    }
    if(event->key() == Qt::Key_Return && Textbox::shift == false) {
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
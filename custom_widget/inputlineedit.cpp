#include "inputlineedit.h"
#include <QString>

InputLineEdit::InputLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setObjectName("inputLineEdit");

    setFixedHeight(32);
}

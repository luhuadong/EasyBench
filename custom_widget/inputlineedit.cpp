#include "inputlineedit.h"
#include <QString>

InputLineEdit::InputLineEdit(QObject *parent) :
    QLineEdit(parent)
{
    setObjectName("inputLineEdit");

    setFixedHeight(32);
}

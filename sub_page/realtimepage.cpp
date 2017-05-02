#include "realtimepage.h"
#include <QTextCodec>

RealtimePage::RealtimePage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Realtime Performance"));
}

#include "realtimepage.h"
#include <QTextCodec>

RealtimePage::RealtimePage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    setTitleLabelText(tr("Realtime Performance"));
}

#include "realtimepage.h"
#include <QTextCodec>

RealtimePage::RealtimePage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    setTitleLabelText(tr("Realtime Performance"));
}

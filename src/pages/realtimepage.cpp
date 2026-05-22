#include "realtimepage.h"
#include <QTextCodec>

RealtimePage::RealtimePage(TbOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    setTitleLabelText(tr("Realtime Performance"));
}

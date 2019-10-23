#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "operationbar.h"
#include "gyt_common.h"
#include "gyt_options.h"

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(GytOptions *options, QWidget *parent = 0);

    void setBackgroundPicture(const QString &path);
    void setTitleLabelText(const QString &text);

    OperationBar *operationBar;

signals:

public slots:

protected:
    GytOptions *g_opt;

private:
    //QWidget *tipsWidget;
    QLabel *titleLabel;

    //QWidget *operationBar;


};

#endif // PAGEWIDGET_H

#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "operationbar.h"
#include "eb_common.h"
#include "eb_options.h"

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(EbOptions *options, QWidget *parent = 0);

    void setBackgroundPicture(const QString &path);
    void setTitleLabelText(const QString &text);

    OperationBar *operationBar;

protected:
    QWidget *contentArea() const;

signals:

public slots:

protected:
    EbOptions *g_opt;

private:
    QLabel *titleLabel;
    QWidget *contentWidget;


};

#endif // PAGEWIDGET_H

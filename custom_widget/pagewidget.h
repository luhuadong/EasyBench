#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "operationbar.h"
#include "gyt_common.h"

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(QWidget *parent = 0);

    void setBackgroundPicture(const QString &path);
    void setTitleLabelText(const QString &text);

    OperationBar *operationBar;

signals:

public slots:

private:
    //QWidget *tipsWidget;
    QLabel *titleLabel;

    //QWidget *operationBar;


};

#endif // PAGEWIDGET_H

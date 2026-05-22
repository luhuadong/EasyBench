#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "tb_common.h"
#include "tb_options.h"

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(TbOptions *options, QWidget *parent = 0);

    void setBackgroundPicture(const QString &path);
    void setTitleLabelText(const QString &text);
    virtual QString defaultStatusHint() const;

    void setStatusMessage(const QString &text);

signals:
    void statusMessageChanged(const QString &text);

protected:
    QWidget *contentArea() const;

protected:
    TbOptions *g_opt;

private:
    QLabel *titleLabel;
    QWidget *contentWidget;
};

#endif // PAGEWIDGET_H

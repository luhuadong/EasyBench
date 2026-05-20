#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "eb_common.h"
#include "eb_options.h"

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(EbOptions *options, QWidget *parent = 0);

    void setBackgroundPicture(const QString &path);
    void setTitleLabelText(const QString &text);
    virtual QString defaultStatusHint() const;

    void setStatusMessage(const QString &text);

signals:
    void statusMessageChanged(const QString &text);

protected:
    QWidget *contentArea() const;

protected:
    EbOptions *g_opt;

private:
    QLabel *titleLabel;
    QWidget *contentWidget;
};

#endif // PAGEWIDGET_H

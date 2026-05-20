#include "pagewidget.h"

#include <QBrush>
#include <QPalette>
#include <QPixmap>
#include <QVBoxLayout>

PageWidget::PageWidget(EbOptions *options, QWidget *parent)
    : QWidget(parent)
{
    g_opt = options;

    setMinimumWidth(CONTENT_WIDTH);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    titleLabel = new QLabel(this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    titleLabel->setAlignment(Qt::AlignCenter);

    contentWidget = new QWidget(this);
    contentWidget->setObjectName(QStringLiteral("pageContent"));
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(contentWidget, 1);
}

QWidget *PageWidget::contentArea() const
{
    return contentWidget;
}

QString PageWidget::defaultStatusHint() const
{
    return tr("就绪");
}

void PageWidget::setStatusMessage(const QString &text)
{
    emit statusMessageChanged(text);
}

void PageWidget::setBackgroundPicture(const QString &path)
{
    contentWidget->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(path)));
    contentWidget->setPalette(palette);
}

void PageWidget::setTitleLabelText(const QString &text)
{
    titleLabel->setText(text);
}

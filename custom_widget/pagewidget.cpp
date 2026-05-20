#include "pagewidget.h"
#include <QPixmap>
#include <QPalette>
#include <QLabel>
#include <QVBoxLayout>

PageWidget::PageWidget(EbOptions *options, QWidget *parent) : QWidget(parent)
{
    g_opt = options;

    setFixedSize(CONTENT_WIDTH, FIXED_WINDOWN_HEIGHT);

    titleLabel = new QLabel(this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    titleLabel->setAlignment(Qt::AlignCenter);

    contentWidget = new QWidget(this);
    contentWidget->setObjectName(QStringLiteral("pageContent"));

    operationBar = new OperationBar(this);
    operationBar->fifthButton()->setText(tr("Return"));

#if LANGUAGE_CHINESE
    operationBar->fifthButton()->setText(tr("返回"));
#endif

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(contentWidget, 1);
    rootLayout->addWidget(operationBar);
}

QWidget *PageWidget::contentArea() const
{
    return contentWidget;
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

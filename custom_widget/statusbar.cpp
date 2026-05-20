#include "statusbar.h"

#include "eb_common.h"

#include <QHBoxLayout>

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("statusBar"));
    setFixedHeight(STATUS_BAR_HEIGHT);

    messageLabel = new QLabel(this);
    messageLabel->setObjectName(QStringLiteral("statusBarMessage"));
    messageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    messageLabel->setText(tr("就绪"));
    messageLabel->setWordWrap(false);
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->addWidget(messageLabel, 1);
}

void StatusBar::setMessage(const QString &text)
{
    messageLabel->setText(text);
}

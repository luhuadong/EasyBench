#include "pagewidget.h"
#include <QPixmap>
#include <QPalette>
#include <QLabel>

PageWidget::PageWidget(QWidget *parent) : QWidget(parent)
{
    //setFixedSize(720, 432);
    setFixedSize(900, 720);
    //setStyleSheet("border-top-left-radius: 4px; border-top-right-radius: 4px;");

    titleLabel = new QLabel(this);
    //titleLabel->setText(tr("###### Hello World ######"));
    titleLabel->setAlignment(Qt::AlignCenter);
    //titleLabel->setGeometry(0, 0, 720, 96);
    titleLabel->setGeometry(0, 0, 900, 96);
    titleLabel->setStyleSheet("border: 2px solid #202020; \
                         border-left-style: transparent; border-right-style: transparent;\
                         border-top-left-radius: 8px; border-top-right-radius: 8px;\
                         background-color: rgba(255, 255, 224, 50%);\
                         font: bold 36px; color: #202020;");


    operationBar = new OperationBar(this);
    //operationBar->setGeometry(0, 432-48, operationBar->width(), operationBar->height());
    operationBar->setGeometry(0, 720-operationBar->height(), operationBar->width(), operationBar->height());
    operationBar->fifthButton()->setText(tr("Return"));

#if LANGUAGE_CHINESE
    operationBar->fifthButton()->setText(tr("返回"));
#endif

}

void PageWidget::setBackgroundPicture(const QString &path)
{
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(path)));
    this->setPalette(palette);
}

void PageWidget::setTitleLabelText(const QString &text)
{
    titleLabel->setText(text);
}

#include "operationbar.h"
#include <QHBoxLayout>

OperationBar::OperationBar(QWidget *parent) :
    QWidget(parent)
{
    //this->setFixedSize(720, 48);
    this->setFixedSize(900, 72);
    //this->setStyleSheet("background-color: yellow; opacity: 200;");


    mainWidget = new QWidget;
    //mainWidget->setObjectName("operatingBar");
    /*mainWidget->setStyleSheet("background-color: Gray;\
                              border: 2px solid transparent;\
                              border-top-color: Black;");*/
    mainWidget->setStyleSheet("background-color:#404040;");

    //mainWidget->setWindowOpacity(0.5); // 调节透明度

    buttonGroup = new QButtonGroup(this);
    createButtons();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(mainWidget);
    setLayout(mainLayout);

}

void OperationBar::createButtons()
{
    //int btnWidth = 144;
    int btnWidth = 180;

    // 为了界面简洁和操作方便，规定操作栏最多五个按钮，可以将其中一个按钮设为“更多”来包含额外的操作。
    for(int i=0; i<5; i++)
    {
        QPushButton *btn = new QPushButton(mainWidget);
        btn->setFixedSize(btnWidth, this->height());
        btn->setObjectName("operationBtn");
        //btn->setStyleSheet("background-color: transparent; border: 1px solid Blue;");
        //btn->setStyleSheet("background-color: White; opacity: 200;");
        //btn->setStyleSheet("color: White; font: bold 24px;");
        //btn->setText(QString::number(i+1));
        btn->setGeometry(btnWidth * i, 0, btn->width(), btn->height());
        buttonGroup->addButton(btn, i+1);
        //btn->setVisible(false);
    }
}

QAbstractButton *OperationBar::firstButton() const
{
    return buttonGroup->button(1);
}

QAbstractButton *OperationBar::secondButton() const
{
    return buttonGroup->button(2);
}

QAbstractButton *OperationBar::thirdButton() const
{
    return buttonGroup->button(3);
}

QAbstractButton *OperationBar::fourthButton() const
{
    return buttonGroup->button(4);
}

QAbstractButton *OperationBar::fifthButton() const
{
    return buttonGroup->button(5);
}

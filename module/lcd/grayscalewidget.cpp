#include "grayscalewidget.h"
#include "eb_common.h"
#include <QLabel>
#include <QVBoxLayout>

GrayscaleWidget::GrayscaleWidget(QSize size, QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(size.width(), size.height());
    setCursor(Qt::BlankCursor);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    for(int i=0; i<16; i++)
    {
        QLabel *label = new QLabel(QString::number(16-i), this);
        label->setAlignment(Qt::AlignCenter);
        //grayscaleVector.append(label);
        //grayscaleList.append(label);
        grayscaleTab[i] = label;
        mainLayout->addWidget(label);
    }

    setLayout(mainLayout);

    grayscaleTab[0]->setStyleSheet("background-color: #000000; color: White");
    grayscaleTab[1]->setStyleSheet("background-color: #101010; color: White");
    grayscaleTab[2]->setStyleSheet("background-color: #202020; color: White");
    grayscaleTab[3]->setStyleSheet("background-color: #303030; color: White");
    grayscaleTab[4]->setStyleSheet("background-color: #404040; color: White");
    grayscaleTab[5]->setStyleSheet("background-color: #505050; color: White");
    grayscaleTab[6]->setStyleSheet("background-color: #606060; color: White");
    grayscaleTab[7]->setStyleSheet("background-color: #707070; color: White");
    grayscaleTab[8]->setStyleSheet("background-color: #808080; color: Black");
    grayscaleTab[9]->setStyleSheet("background-color: #909090; color: Black");
    grayscaleTab[10]->setStyleSheet("background-color: #A0A0A0; color: Black");
    grayscaleTab[11]->setStyleSheet("background-color: #B0B0B0; color: Black");
    grayscaleTab[12]->setStyleSheet("background-color: #C0C0C0; color: Black");
    grayscaleTab[13]->setStyleSheet("background-color: #D0D0D0; color: Black");
    grayscaleTab[14]->setStyleSheet("background-color: #E0E0E0; color: Black");
    grayscaleTab[15]->setStyleSheet("background-color: #F0F0F0; color: Black");
}

GrayscaleWidget::~GrayscaleWidget()
{
    for(int i=0; i<16; i++)
    {
        delete grayscaleTab[i];
    }
}

void GrayscaleWidget::mousePressEvent(QMouseEvent *)
{
    this->close();
}

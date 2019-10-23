#include "touchpage.h"
#include "gyt_common.h"
#include <QTextCodec>
#include <QProcess>
#include <QGridLayout>


TouchPage::TouchPage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("Touchscreen Calibration & Test"));
    setTitleLabelText(tr("触摸校准及测试"));

    calBtn = new QPushButton(tr("4 points calibration"), this);
    calBtn->setObjectName("functionBtn");
    calBtn->setFixedSize(250, 150);

    linz9Btn = new QPushButton(tr("9 points calibration"), this);
    linz9Btn->setObjectName("functionBtn");
    linz9Btn->setFixedSize(250, 150);

    linzBtn = new QPushButton(tr("25 points calibration"), this);
    linzBtn->setObjectName("functionBtn");
    linzBtn->setFixedSize(250, 150);

    drawBtn = new QPushButton(tr("Draw Test"), this);
    drawBtn->setObjectName("functionBtn");
    drawBtn->setFixedSize(250, 150);

    connect(calBtn, SIGNAL(clicked()), this, SLOT(calBtnClicked()));
    connect(linz9Btn, SIGNAL(clicked()), this, SLOT(linz9BtnClicked()));
    connect(linzBtn, SIGNAL(clicked()), this, SLOT(linzBtnClicked()));
    connect(drawBtn, SIGNAL(clicked()), this, SLOT(drawBtnClicked()));

    if(TOUCH_NONE == g_opt->getTouchType()) {
        calBtn->setEnabled(false);
        linz9Btn->setEnabled(false);
        linzBtn->setEnabled(false);
        drawBtn->setEnabled(false);
    }
    else if(TOUCH_CAPACITIVE == g_opt->getTouchType()) {
        calBtn->setEnabled(false);
        linz9Btn->setEnabled(false);
        linzBtn->setEnabled(false);
        drawBtn->setEnabled(true);
    }
    else {
        calBtn->setEnabled(true);
        linz9Btn->setEnabled(true);
        linzBtn->setEnabled(true);
        drawBtn->setEnabled(true);
    }

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(calBtn, 0, 0, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(linz9Btn, 0, 1, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(linzBtn, 1, 0, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(drawBtn, 1, 1, 1, 1, Qt::AlignCenter);
    mainLayout->setContentsMargins(30, 100, 30, 60);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);

    operationBar->firstButton()->setEnabled(false);
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

#if LANGUAGE_CHINESE
    calBtn->setText(tr("4点校准"));
    linz9Btn->setText(tr("9点校准"));
    linzBtn->setText(tr("25点校准"));
    drawBtn->setText(tr("轨迹测试"));
#endif
}

void TouchPage::calBtnClicked()
{
    QString program = "eCalib";
    QStringList arguments;
    arguments << "Cal";
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void TouchPage::linz9BtnClicked()
{
    QString program = "eCalib";
    QStringList arguments;
    arguments << "Linz9";
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void TouchPage::linzBtnClicked()
{
    QString program = "eCalib";
    QStringList arguments;
    arguments << "Linz";
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void TouchPage::drawBtnClicked()
{
    QString program = "eCalib";
    QStringList arguments;
    arguments << "Draw";
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

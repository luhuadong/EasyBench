#include "lcdpage.h"
#include <QTextCodec>
#include <QFile>
#include <QByteArray>
#include <QVBoxLayout>
#include <QSettings>
#include <QApplication>


LcdPage::LcdPage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("LCD Screen Test"));
    setTitleLabelText(tr("液晶显示屏测试"));

    pixelCheckBtn = new QPushButton(tr("Pixel Check"), this);
    pixelCheckBtn->setObjectName("functionBtn");
    grayscaleTestBtn = new QPushButton(tr("Grayscale Test"), this);
    grayscaleTestBtn->setObjectName("functionBtn");

    pixelCheckBtn->setGeometry(150, 260+30, 200, 200);
    grayscaleTestBtn->setGeometry(550, 260+30, 200, 200);

    connect(pixelCheckBtn, SIGNAL(clicked()), this, SLOT(pixelCheckBtnClicked()));
    connect(grayscaleTestBtn, SIGNAL(clicked()), this, SLOT(grayscaleTestBtnClicked()));

    operationBar->secondButton()->setText(tr("Backlight Down"));
    operationBar->thirdButton()->setText(tr("Backlight Up"));
    operationBar->firstButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(lcdBacklightDown()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(lcdBacklightUp()));

    backlightName = g_opt->getBacklightNode();
    QByteArray byteArray;
    QFile backlightFile(QString("/sys/class/backlight/%1/brightness").arg(backlightName));
    backlightFile.open(QFile::ReadOnly);
    byteArray = backlightFile.readAll();
    //backlightValue = backlightFile.readAll().toInt();
    backlightValue = QString(byteArray).toInt();
    backlightFile.close();

    MinBacklightValue = 1;
    MaxBacklightValue = 100;
    backlightFile.setFileName(QString("/sys/class/backlight/%1/max_brightness").arg(backlightName));
    backlightFile.open(QFile::ReadOnly);
    byteArray = backlightFile.readAll();
    MaxBacklightValue = QString(byteArray).toInt();
    backlightFile.close();

    // Backlight Level
    backlightLabel = new QLabel(tr("背光等级 : ") + QString::number(backlightValue), this);
    backlightLabel->setGeometry(100, 550, 150, 50);

    backlightBar = new QProgressBar(this);
    backlightBar->setMinimum(MinBacklightValue);
    backlightBar->setMaximum(MaxBacklightValue);
    backlightBar->setValue(backlightValue);
    backlightBar->setGeometry(250, 550, 550, 50);

    lcdInfoBox = new QGroupBox(this);
    lcdInfoBox->setTitle(tr("LCD Info"));
    lcdInfoBox->setGeometry(100, 120, 700, 140);
    lcdInfoLabel = new QLabel(lcdInfoBox);
    lcdInfoLabel->setText(tr("12 英寸 TFT LCD, made in China."));
    resolutionLabel = new QLabel(lcdInfoBox);
    bppLabel = new QLabel(lcdInfoBox);

    QVBoxLayout *lcdInfoLayout = new QVBoxLayout;
    lcdInfoLayout->setContentsMargins(40, 10, 40, 10);
    lcdInfoLayout->addWidget(lcdInfoLabel);
    lcdInfoLayout->addWidget(resolutionLabel);
    lcdInfoLayout->addWidget(bppLabel);
    lcdInfoBox->setLayout(lcdInfoLayout);

    if(-1 == getScreenInfo()) {
        printf("Can not get screen information.\n");
    }
    else {
        /* Screen fix information */
        //lcdInfoLabel->setText(tr());
        // Visible resolution
        resolutionLabel->setText(tr("可视分辨率 : ") + QString::number(vinfo.xres) + tr(" x ") + QString::number(vinfo.yres));
        // Bits per pixel
        bppLabel->setText(tr("BPP值 : ") + QString::number(vinfo.bits_per_pixel) + tr(" bpp"));
        //grayscaleLabel->setText(QString("Grayscale : ") + QString::number(vinfo.grayscale));
        //pixclockLabel->setText(QString("Pixclock : ") + QString::number(vinfo.pixclock));
        //sizeLabel->setText(QString("Size : ") + QString::number(vinfo.height) + QString(" x ") + QString::number(vinfo.width));
    }

#if LANGUAGE_CHINESE
    lcdInfoBox->setTitle(tr("液晶屏参数"));
    operationBar->secondButton()->setText(tr("亮度减"));
    operationBar->thirdButton()->setText(tr("亮度加"));
    pixelCheckBtn->setText(tr("像素测试"));
    grayscaleTestBtn->setText(tr("灰度测试"));
#endif

}

int LcdPage::getScreenInfo()
{
    int fbfd = 0;
    fbfd = open("/dev/fb0", O_RDONLY);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return -1;
    }
    printf("The framebuffer device was opened successfully.\n");

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return -1;
    }

    ::close(fbfd);
    return 0;
}

void LcdPage::pixelCheckBtnClicked()
{
    /*
    QWidget *pixelWidget = new QWidget(0);
    pixelWidget->setWindowFlags(Qt::FramelessWindowHint);
    pixelWidget->setFixedSize(1024, 768);
    pixelWidget->setStyleSheet("background-color: Blue;");
    */
    colorBtn = new QPushButton;
    colorBtn->setObjectName("noneOutlineBtn");
    colorBtn->setFixedSize(g_opt->lcdSize().width(), g_opt->lcdSize().height());
    colorBtn->setWindowFlags(Qt::FramelessWindowHint);
    colorBtn->setStyleSheet("background-color: #0000FF");
    colorBtn->setCursor(Qt::BlankCursor);
    colorBtn->show();

    connect(colorBtn, SIGNAL(pressed()), this, SLOT(changeColor()));
}

void LcdPage::grayscaleTestBtnClicked()
{
    GrayscaleWidget *grayscaleWidget = new GrayscaleWidget(g_opt->lcdSize());
    grayscaleWidget->show();
}

void LcdPage::changeColor()
{
    static int cnt = 1;

    switch(cnt)
    {
        case 0: colorBtn->setStyleSheet("background-color: #0000FF"); break;
        case 1: colorBtn->setStyleSheet("background-color: #FF0000"); break;
        case 2: colorBtn->setStyleSheet("background-color: #00FF00"); break;
        case 3: colorBtn->setStyleSheet("background-color: #FFFF00"); break;
        case 4: colorBtn->setStyleSheet("background-color: #00FFFF"); break;
        case 5: colorBtn->setStyleSheet("background-color: #FF00FF"); break;
        case 6: colorBtn->setStyleSheet("background-color: #808080"); break;
        case 7: colorBtn->setStyleSheet("background-color: #FFFFFF"); break;
        case 8: colorBtn->setStyleSheet("background-color: #000000"); break;
        case 9: disconnect(colorBtn, SIGNAL(clicked()), this, SLOT(colorBtnClicked())); colorBtn->close(); cnt = 0; break;
        default: break;
    }
    cnt++;
}

void LcdPage::lcdBacklightUp()
{
    if(backlightValue < MaxBacklightValue)
    {
        char cmd[128];
        backlightValue++;
        backlightLabel->setText(tr("背光等级 : ") + QString::number(backlightValue));
        backlightBar->setValue(backlightValue);
        sprintf(cmd, "echo %d > /sys/class/backlight/%s/brightness", backlightValue, g_opt->getBacklightNode().toLatin1().data());
        system(cmd);
    }
}

void LcdPage::lcdBacklightDown()
{
    if(backlightValue > MinBacklightValue)
    {
        char cmd[128];
        backlightValue--;
        backlightLabel->setText(tr("背光等级 : ") + QString::number(backlightValue));
        backlightBar->setValue(backlightValue);
        sprintf(cmd, "echo %d > /sys/class/backlight/%s/brightness", backlightValue, g_opt->getBacklightNode().toLatin1().data());
        system(cmd);
    }
}

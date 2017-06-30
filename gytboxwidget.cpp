#include "gytboxwidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QFile>
#include <QDebug>

GytBoxWidget::GytBoxWidget(QWidget *parent)
    : QWidget(parent)
{
//    QString cfgFileName = QString(QApplication::applicationDirPath() +"/gyt_box.conf");
//    QSettings configRead(cfgFileName, QSettings::IniFormat);
//    backlightName = configRead.value("/LCD/Backlight_name").toString();

    setWindowTitle(tr("GYT Box"));
    //setFixedSize(1024, 768);
    setFixedSize(LCD_WIDTH, LCD_HEIGHT - TITLE_HEIGHT);
    //setWindowFlags(Qt::FramelessWindowHint);

    //---------- 标题栏 -----------
    /*
    titleWidget = new QWidget(this);
    titleWidget->setGeometry(0, 0, 1024, 48);
    titleWidget->setObjectName("titleWidget");

    closeBtn = new QPushButton(tr("close"), titleWidget);
    closeBtn->setGeometry(1024-100, 4, 96, 40);
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));
    */

    //---------- 菜单栏 -----------

    menuWidget = new QWidget(this);
    menuWidget->setGeometry(0, 0, 124, 720);
    menuWidget->setObjectName("menuWidget");
    //menuWidget->setStyleSheet("background-color: #202020");

    menuBtnGroup = new QButtonGroup(this);
    QSize size(124, 90);

    QStringList menuList;
    menuList << tr("LCD") << tr("Touch") << tr("Camera") << tr("Setting")
             << tr("Monitor") << tr("Version") << tr("RealTime") << tr("About");

    for(int i=0; i<8; i++)
    {
        QToolButton *menuBtn = new QToolButton(menuWidget);
        menuBtn->setObjectName("menuBtn");
        menuBtnGroup->addButton(menuBtn, PAGE_LCD + i);    // 将自定义的button加入customGroup中，并为其设置id

        //menuBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); //文字处于图片下方
        //QPixmap pixmap(":/images/title_icon_48.png");
        //QPixmap pixmap2(QString(":/images/menu_%1.png").arg(i+1));
        //menuBtn->setIcon(pixmap2);   // 为按钮设置图标
        //menuBtn->setIconSize(size);   // 设置图片大小
        menuBtn->setFixedSize(size.width(), size.height()); // 设置按钮大小
        menuBtn->setText(menuList.at(i)); // 设置提示文字
        menuBtn->setCheckable(true);
        menuBtn->setChecked(false);
        //menuBtn->setStyleSheet(menuBtnUncheckedSS); // 所有按钮初始状态为unchecked

        //customBtnBarLayout->addWidget(menuBtn);   // 添加到布局
        menuBtn->setGeometry(0, i*size.height(), size.width(), size.height());
        //menuBtn->show();
    }

    //------------------- Page Widget ----------------------
    centerPages = new QStackedWidget(this);
    centerPages->setGeometry(124, 0, 900, 720);

    lcdPage = new LcdPage(this);
    touchPage = new TouchPage(this);
    cameraPage = new CameraPage(this);
    datetimePage = new DatetimePage(this);
    monitorPage = new MonitorPage(this);
    versionPage = new VersionPage(this);
    realtimePage = new RealtimePage(this);
    aboutPage = new AboutPage(this);

    centerPages->addWidget(lcdPage);
    centerPages->addWidget(touchPage);
    centerPages->addWidget(cameraPage);
    centerPages->addWidget(datetimePage);
    centerPages->addWidget(monitorPage);
    centerPages->addWidget(versionPage);
    centerPages->addWidget(realtimePage);
    centerPages->addWidget(aboutPage);

    centerPages->setCurrentWidget(aboutPage);

    // 系统配置
    prevPage = PAGE_ABOUT;
    currPage = PAGE_ABOUT;

    connect(menuBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(menuBtnGroupToggled(int)));


    /*
    QLabel *label = new QLabel(tr("Hello World !"), this);
    label->setFixedSize(1024, 720);
    label->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setBold(true);
    font.setPixelSize(36);
    label->setFont(font);
    label->setStyleSheet("border-style: outset; border-width: 8px; border-color: red; color: Blue; background-color: yellow");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(label);
    setLayout(mainLayout);
    */
}

GytBoxWidget::~GytBoxWidget()
{
    system("rm /home/root/.lock_gyt_box");
}

void GytBoxWidget::menuBtnGroupToggled(int id)
{
    centerPages->setCurrentIndex(id);
}

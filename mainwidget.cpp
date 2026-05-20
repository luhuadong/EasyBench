#include "mainwidget.h"
#include "eb_branding.h"
#include "eb_common.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QToolButton>
#include <QIcon>

struct MenuItemDef {
    int pageId;
    const char *icon;
    const char *iconActive;
};

static const MenuItemDef kMenuItems[] = {
    { MainWidget::PAGE_SYSTEM,   ":/icons/menu_system.svg",   ":/icons/menu_system_active.svg" },
    { MainWidget::PAGE_LCD,      ":/icons/menu_lcd.svg",      ":/icons/menu_lcd_active.svg" },
    { MainWidget::PAGE_CAMERA,   ":/icons/menu_camera.svg",   ":/icons/menu_camera_active.svg" },
    { MainWidget::PAGE_NETWORK,  ":/icons/menu_network.svg",  ":/icons/menu_network_active.svg" },
    { MainWidget::PAGE_SERIAL,   ":/icons/menu_serial.svg",   ":/icons/menu_serial_active.svg" },
    { MainWidget::PAGE_STORAGE,  ":/icons/menu_storage.svg",  ":/icons/menu_storage_active.svg" },
    { MainWidget::PAGE_AUDIO,    ":/icons/menu_audio.svg",    ":/icons/menu_audio_active.svg" },
    { MainWidget::PAGE_VERSION,  ":/icons/menu_version.svg",  ":/icons/menu_version_active.svg" },
};

MainWidget::MainWidget(EbOptions *options, QWidget *parent)
    : QWidget(parent)
{
    g_opt = options;

    setWindowIcon(EbBranding::applicationIcon());

    initMainUI();
    connect(menuBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(menuBtnGroupToggled(int)));
    connect(menuBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMenuButtonIcons()));
    updateMenuButtonIcons();
}

MainWidget::~MainWidget()
{
}

void MainWidget::initMainUI()
{
    setWindowTitle(QStringLiteral("EasyBench"));
#if FIXED_WINDOWN
    setFixedSize(g_opt->fixedSize().width(), g_opt->fixedSize().height());
#else
    setFixedSize(LCD_WIDTH, LCD_HEIGHT - TITLE_HEIGHT);
#endif

    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    menuWidget = new QWidget(this);
    menuWidget->setObjectName(QStringLiteral("menuWidget"));
    menuWidget->setFixedWidth(SIDEBAR_WIDTH);

    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0, 8, 0, 8);
    menuLayout->setSpacing(2);

    menuBtnGroup = new QButtonGroup(this);
    menuBtnGroup->setExclusive(true);

    const QStringList menuLabels = QStringList()
        << tr("系统") << tr("显示") << tr("相机") << tr("网络")
        << tr("串口") << tr("存储") << tr("声音") << tr("版本");

    const int menuCount = sizeof(kMenuItems) / sizeof(kMenuItems[0]);
    for (int i = 0; i < menuCount; ++i) {
        QToolButton *menuBtn = new QToolButton(menuWidget);
        menuBtn->setObjectName(QStringLiteral("menuBtn"));
        menuBtnGroup->addButton(menuBtn, kMenuItems[i].pageId);

        menuBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        menuBtn->setIcon(QIcon(QString::fromUtf8(kMenuItems[i].icon)));
        menuBtn->setIconSize(QSize(24, 24));
        menuBtn->setText(menuLabels.at(i));
        menuBtn->setToolTip(menuLabels.at(i));
        menuBtn->setCheckable(true);
        menuBtn->setChecked(i == 0);
        menuBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        menuBtn->setMinimumHeight(40);
        menuBtn->setProperty("iconNormal", kMenuItems[i].icon);
        menuBtn->setProperty("iconActive", kMenuItems[i].iconActive);

        menuLayout->addWidget(menuBtn, 1);
    }

    centerPages = new QStackedWidget(this);
    centerPages->setObjectName(QStringLiteral("centerPages"));

    rootLayout->addWidget(menuWidget);
    rootLayout->addWidget(centerPages, 1);

    systemPage = new SystemPage(g_opt, this);
    lcdPage = new LcdPage(g_opt, this);
    cameraPage = new CameraPage(g_opt, this);
    networkPage = new NetworkPage(g_opt, this);
    serialPage = new SerialPage(g_opt, this);
    storagePage = new StoragePage(g_opt, this);
    audioPage = new AudioPage(g_opt, this);
    versionPage = new VersionPage(g_opt, this);

    centerPages->addWidget(systemPage);
    centerPages->addWidget(lcdPage);
    centerPages->addWidget(cameraPage);
    centerPages->addWidget(networkPage);
    centerPages->addWidget(serialPage);
    centerPages->addWidget(storagePage);
    centerPages->addWidget(audioPage);
    centerPages->addWidget(versionPage);

    centerPages->setCurrentWidget(systemPage);

    prevPage = PAGE_SYSTEM;
    currPage = PAGE_SYSTEM;
}

void MainWidget::menuBtnGroupToggled(int id)
{
    centerPages->setCurrentIndex(id);
    currPage = static_cast<pageTypes>(id);
}

void MainWidget::updateMenuButtonIcons()
{
    const QList<QAbstractButton *> buttons = menuBtnGroup->buttons();
    for (QAbstractButton *button : buttons) {
        QToolButton *menuBtn = qobject_cast<QToolButton *>(button);
        if (!menuBtn) {
            continue;
        }
        const QString iconPath = menuBtn->isChecked()
            ? menuBtn->property("iconActive").toString()
            : menuBtn->property("iconNormal").toString();
        if (!iconPath.isEmpty()) {
            menuBtn->setIcon(QIcon(iconPath));
        }
    }
}

#include "mainwidget.h"
#include "tb_branding.h"
#include "tb_common.h"
#include "widgets/tb_widget_util.h"

#include <QAbstractButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>

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
    { MainWidget::PAGE_UPGRADE,  ":/icons/menu_update.svg",   ":/icons/menu_update_active.svg" },
    { MainWidget::PAGE_VERSION,  ":/icons/menu_version.svg",  ":/icons/menu_version_active.svg" },
};

MainWidget::MainWidget(TbOptions *options, QWidget *parent)
    : QWidget(parent)
{
    g_opt = options;

    setWindowIcon(TbBranding::applicationIcon());

    initMainUI();
    connect(menuBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(menuBtnGroupToggled(int)));
    connect(menuBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMenuButtonIcons()));
    updateMenuButtonIcons();
    applyPageDefaultStatus(PAGE_SYSTEM);
}

MainWidget::~MainWidget()
{
}

void MainWidget::initMainUI()
{
    setWindowTitle(QStringLiteral("TuxiBit"));
#if FIXED_WINDOWN
    setFixedSize(g_opt->fixedSize().width(), g_opt->fixedSize().height());
#else
    const QSize minSize = g_opt->fixedSize();
    setMinimumSize(minSize);
    resize(minSize);
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
        << tr("串口") << tr("存储") << tr("声音") << tr("升级") << tr("版本");

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

    contentColumn = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentColumn);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    centerPages = new QStackedWidget(contentColumn);
    centerPages->setObjectName(QStringLiteral("centerPages"));

    statusBar = new StatusBar(contentColumn);

    contentLayout->addWidget(centerPages, 1);
    contentLayout->addWidget(statusBar);

    rootLayout->addWidget(menuWidget);
    rootLayout->addWidget(contentColumn, 1);

    systemPage = new SystemPage(g_opt, this);
    lcdPage = new LcdPage(g_opt, this);
    cameraPage = new CameraPage(g_opt, this);
    networkPage = new NetworkPage(g_opt, this);
    serialPage = new SerialPage(g_opt, this);
    storagePage = new StoragePage(g_opt, this);
    audioPage = new AudioPage(g_opt, this);
    versionPage = new VersionPage(g_opt, this);
    upgradePage = new UpgradePage(g_opt, this);

    centerPages->addWidget(systemPage);
    centerPages->addWidget(lcdPage);
    centerPages->addWidget(cameraPage);
    centerPages->addWidget(networkPage);
    centerPages->addWidget(serialPage);
    centerPages->addWidget(storagePage);
    centerPages->addWidget(audioPage);
    centerPages->addWidget(versionPage);
    centerPages->addWidget(upgradePage);

    bindPageStatus(systemPage);
    bindPageStatus(lcdPage);
    bindPageStatus(cameraPage);
    bindPageStatus(networkPage);
    bindPageStatus(serialPage);
    bindPageStatus(storagePage);
    bindPageStatus(audioPage);
    bindPageStatus(versionPage);
    bindPageStatus(upgradePage);

    centerPages->setCurrentWidget(systemPage);

    prevPage = PAGE_SYSTEM;
    currPage = PAGE_SYSTEM;

    const int popupHeight = PAGE_BODY_HEIGHT - STATUS_BAR_HEIGHT - 48;
    TbWidget::applyComboBoxStyles(this, popupHeight);
    TbWidget::applyFormFieldStyles(this);
}

void MainWidget::bindPageStatus(PageWidget *page)
{
    connect(page, &PageWidget::statusMessageChanged, statusBar, &StatusBar::setMessage);
}

void MainWidget::applyPageDefaultStatus(pageTypes page)
{
    PageWidget *widget = pageForType(page);
    if (widget && statusBar) {
        statusBar->setMessage(widget->defaultStatusHint());
    }
}

PageWidget *MainWidget::pageForType(pageTypes page) const
{
    switch (page) {
    case PAGE_SYSTEM:
        return systemPage;
    case PAGE_LCD:
        return lcdPage;
    case PAGE_CAMERA:
        return cameraPage;
    case PAGE_NETWORK:
        return networkPage;
    case PAGE_SERIAL:
        return serialPage;
    case PAGE_STORAGE:
        return storagePage;
    case PAGE_AUDIO:
        return audioPage;
    case PAGE_VERSION:
        return versionPage;
    case PAGE_UPGRADE:
        return upgradePage;
    }
    return nullptr;
}

void MainWidget::menuBtnGroupToggled(int id)
{
    centerPages->setCurrentIndex(id);
    currPage = static_cast<pageTypes>(id);
    applyPageDefaultStatus(currPage);
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

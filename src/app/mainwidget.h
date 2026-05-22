#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QToolButton>

#include "tb_options.h"
#include "widgets/pagewidget.h"
#include "widgets/statusbar.h"
#include "pages/lcdpage.h"
#include "pages/camerapage.h"
#include "pages/datetimepage.h"
#include "pages/versionpage.h"
#include "pages/upgradepage.h"
#include "pages/realtimepage.h"
#include "pages/audiopage.h"
#include "pages/networkpage.h"
#include "pages/serialpage.h"
#include "pages/storagepage.h"
#include "pages/systempage.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(TbOptions *options, QWidget *parent = 0);
    ~MainWidget();

    typedef enum {
        PAGE_SYSTEM = 0,
        PAGE_LCD,
        PAGE_CAMERA,
        PAGE_NETWORK,
        PAGE_SERIAL,
        PAGE_STORAGE,
        PAGE_AUDIO,
        PAGE_VERSION,
        PAGE_UPGRADE,
    } pageTypes;

private slots:
    void menuBtnGroupToggled(int);
    void updateMenuButtonIcons();

private:
    void initMainUI();
    void bindPageStatus(PageWidget *page);
    void applyPageDefaultStatus(pageTypes page);
    PageWidget *pageForType(pageTypes page) const;

    TbOptions *g_opt;

    QWidget *menuWidget;
    QButtonGroup *menuBtnGroup;
    QWidget *contentColumn = nullptr;
    QStackedWidget *centerPages;
    StatusBar *statusBar = nullptr;

    SystemPage *systemPage;
    LcdPage *lcdPage;
    CameraPage *cameraPage;
    NetworkPage *networkPage;
    SerialPage *serialPage;
    StoragePage *storagePage;
    AudioPage *audioPage;
    VersionPage *versionPage;
    UpgradePage *upgradePage;

    pageTypes prevPage;
    pageTypes currPage;
};

#endif // MAINWIDGET_H

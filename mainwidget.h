#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QToolButton>

#include "eb_options.h"
#include "custom_widget/pagewidget.h"
#include "custom_widget/statusbar.h"
#include "sub_page/lcdpage.h"
#include "sub_page/camerapage.h"
#include "sub_page/datetimepage.h"
#include "sub_page/versionpage.h"
#include "sub_page/upgradepage.h"
#include "sub_page/realtimepage.h"
#include "sub_page/audiopage.h"
#include "sub_page/networkpage.h"
#include "sub_page/serialpage.h"
#include "sub_page/storagepage.h"
#include "sub_page/systempage.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(EbOptions *options, QWidget *parent = 0);
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

    EbOptions *g_opt;

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

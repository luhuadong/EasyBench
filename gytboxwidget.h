#ifndef GYTBOXWIDGET_H
#define GYTBOXWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>

#include "gyt_options.h"
#include "sub_page/lcdpage.h"
#include "sub_page/touchpage.h"
#include "sub_page/camerapage.h"
#include "sub_page/datetimepage.h"
#include "sub_page/monitorpage.h"
#include "sub_page/versionpage.h"
#include "sub_page/realtimepage.h"
#include "sub_page/seatpage.h"
#include "sub_page/networkpage.h"
#include "sub_page/systempage.h"


class GytBoxWidget : public QWidget
{
    Q_OBJECT

public:
    GytBoxWidget(GytOptions *options, QWidget *parent = 0);
    ~GytBoxWidget();

    typedef enum {
        PAGE_LCD = 0,
        PAGE_TOUCH,
        PAGE_CAMERA,
        //PAGE_DATETIME,
        PAGE_NETWORK,
        PAGE_SERIALPORT,
        PAGE_ABOUT,  // Settings Page
        PAGE_MONITOR,
        PAGE_VERSION,
        //PAGE_REALTIME,

    }pageTypes;

private slots:

    void menuBtnGroupToggled(int);
    //void showHomePage(void);
    //void sideBtnToggled(bool);

private:
    void initMainUI();

    GytOptions *g_opt;

    //---------- 标题栏 -----------
    //QWidget *titleWidget;
    //QPushButton *closeBtn;

    //---------- 菜单栏 -----------
    QWidget *menuWidget;
    QButtonGroup *menuBtnGroup;

    //---------- 侧边栏 -----------
    //QWidget *sideBar;

    //---------- 子界面 -----------
    QStackedWidget *centerPages;

    LcdPage     *lcdPage;
    TouchPage   *touchPage;
    CameraPage  *cameraPage;
    NetworkPage *networkPage;
    SystemPage  *systemPage;
    SeatPage    *seatPage;
    MonitorPage *monitorPage;
    VersionPage *versionPage;

    pageTypes prevPage;
    pageTypes currPage;

};

#endif // GYTBOXWIDGET_H

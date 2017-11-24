#ifndef GYTBOXWIDGET_H
#define GYTBOXWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>

#include "sub_page/lcdpage.h"
#include "sub_page/touchpage.h"
#include "sub_page/camerapage.h"
#include "sub_page/datetimepage.h"
#include "sub_page/monitorpage.h"
#include "sub_page/versionpage.h"
#include "sub_page/realtimepage.h"
#include "sub_page/aboutpage.h"
#include "sub_page/networkpage.h"
#include "sub_page/serialportpage.h"


class GytBoxWidget : public QWidget
{
    Q_OBJECT

public:
    GytBoxWidget(QWidget *parent = 0);
    ~GytBoxWidget();

    typedef enum {
        PAGE_LCD = 0,
        PAGE_TOUCH,
        PAGE_CAMERA,
        //PAGE_DATETIME,
        PAGE_NETWORK,
        PAGE_SERIALPORT,
        PAGE_MONITOR,
        PAGE_VERSION,
        //PAGE_REALTIME,
        PAGE_ABOUT,
    }pageTypes;

private slots:
    void menuBtnGroupToggled(int);
    //void showHomePage(void);
    //void sideBtnToggled(bool);

private:

    //---------- 标题栏 -----------
    //QWidget *titleWidget;
    //QPushButton *closeBtn;

    //---------- 菜单栏 -----------
    QWidget *menuWidget;
    QButtonGroup *menuBtnGroup;

    /* **********************************************************************
     * 1: LCD                             2: TouchScreen    3: Camera               4: Date-Time & Time-Zone
     * 5: Resources Monitor   6: Version Info     7: RealTime Test     8: About Author
     ************************************************************************/

    //---------- 侧边栏 -----------
    //QWidget *sideBar;

    //---------- 子界面 -----------
    QStackedWidget *centerPages;

    LcdPage *lcdPage;
    TouchPage *touchPage;
    CameraPage *cameraPage;
    //DatetimePage *datetimePage;
    NetworkPage *networkPage;
    SerialPortPage *serialPortPage;
    MonitorPage *monitorPage;
    VersionPage *versionPage;
    //RealtimePage *realtimePage;
    AboutPage *aboutPage;


    pageTypes prevPage;
    pageTypes currPage;

};

#endif // GYTBOXWIDGET_H

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>

#include "eb_options.h"
#include "sub_page/lcdpage.h"
#include "sub_page/camerapage.h"
#include "sub_page/datetimepage.h"
#include "sub_page/versionpage.h"
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
    } pageTypes;

private slots:
    void menuBtnGroupToggled(int);
    void updateMenuButtonIcons();

private:
    void initMainUI();

    EbOptions *g_opt;

    QWidget *menuWidget;
    QButtonGroup *menuBtnGroup;
    QStackedWidget *centerPages;

    SystemPage  *systemPage;
    LcdPage     *lcdPage;
    CameraPage  *cameraPage;
    NetworkPage *networkPage;
    SerialPage  *serialPage;
    StoragePage *storagePage;
    AudioPage   *audioPage;
    VersionPage *versionPage;

    pageTypes prevPage;
    pageTypes currPage;
};

#endif // MAINWIDGET_H

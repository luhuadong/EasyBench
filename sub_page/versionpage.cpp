#include "versionpage.h"
#include "eb_common.h"
#include <QTextCodec>
#include <QStringList>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>

#include "custom_widget/updatedialog.h"

#define TAB_ITEM_HEIGHT 48

VersionPage::VersionPage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("Version Information"));
    setTitleLabelText(tr("设备版本信息"));

    //-------------- table ----------------
    tabView = new QTableView(this);

    tabView->setGeometry(0+80, 96+36+24, 900-160, 9 * TAB_ITEM_HEIGHT + 2);
    tabModel = new QStandardItemModel;
    //itemCntLabel->setProperty("h", 6);

    //tabModel->setColumnCount(3);
    //tabModel->setHeaderData(0, Qt::Horizontal, tr("Error code"));
    //tabModel->setHeaderData(1, Qt::Horizontal, tr("Description"));
    //tabModel->setHeaderData(2, Qt::Horizontal, tr("Date time"));
    tabModel->setColumnCount(2);

    tabView->setModel(tabModel);
    tabView->verticalHeader()->hide();   // 隐藏垂直表头
    tabView->horizontalHeader()->hide();
    tabView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 不显示垂直滚动条
    tabView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 不显示水平滚动条
    //tabView->horizontalHeader()->setSectionsClickable(false);    // 设置水平表头不可点击
    tabView->setSelectionBehavior(QAbstractItemView::SelectRows);    // 设置每次点击选中一整行
    tabView->setSelectionMode(QAbstractItemView::SingleSelection);   // 设置每次只能选中一行
    tabView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置为不可编辑
    tabView->setShowGrid(true); // 设置显示网格线
//    tabView->setFrameShape(QFrame::Box);
    QFont font = tabView->horizontalHeader()->font();    // 设置水平表头字体加粗
    font.setBold(true);
    tabView->horizontalHeader()->setFont(font);
    tabView->horizontalHeader()->setFixedHeight(34);
    tabView->verticalHeader()->setDefaultSectionSize(TAB_ITEM_HEIGHT);   // 改变默认行高
    //tabView->horizontalHeader()->setTextElideMode(Qt::ElideLeft);    // 超出显示区域时省略号在左边
    tabView->setColumnWidth(0, 150); // 设置第一列宽度为150
    //tabView->setColumnWidth(1, 400); // 设置第二列宽度为400
    tabView->horizontalHeader()->setStretchLastSection(true);    // 设置最后一列充满表宽度

    //-----------------------------


#if LANGUAGE_CHINESE
    operationBar->firstButton()->setText(tr("关机"));
    operationBar->secondButton()->setText(tr("重启"));
    operationBar->thirdButton()->setText(tr("升级"));
#else
    operationBar->firstButton()->setText(tr("Shutdown"));
    operationBar->secondButton()->setText(tr("Reboot"));
    operationBar->thirdButton()->setText(tr("Update"));
#endif
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->firstButton(), SIGNAL(clicked()), this, SLOT(shutdownSystem()));
    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(rebootSystem()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(updatePackage()));

    getVersionData();
}

void VersionPage::getVersionData()
{
    QStringList itemNameList;
    itemNameList << tr("设备型号") << tr("系统版本") << tr("发行版本") << tr("内核版本") << tr("Uboot版本")
                 << tr("GCC版本") << tr("核心模块") << tr("开发者") << tr("Easy Bench");

    QStringList itemValueList;
    itemValueList << g_opt->getProductInfo()
                  << g_opt->getGYOSInfo()
                  << g_opt->getDistroInfo()
                  << g_opt->getKernelInfo()
                  << g_opt->getBootloaderInfo()
                  << g_opt->getGCCInfo()
                  << g_opt->getModelInfo()
                  << g_opt->getDeveloperInfo()
                  << g_opt->getAppVersion();

    for (int row = 0; row < itemNameList.count(); ++row) {
        for (int column = 0; column < 2; ++column) {
            if(column == 0) {
                QStandardItem *item = new QStandardItem(itemNameList.at(row));
                item->setTextAlignment(Qt::AlignCenter);
                tabModel->setItem(row, column, item);
            }
            else if(column == 1) {
                QStandardItem *item = new QStandardItem(itemValueList.at(row));
                tabModel->setItem(row, column, item);
            }
            //QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            //tabModel->setItem(row, column, item);
        }
    }
}

void VersionPage::shutdownSystem()
{
    system("shutdown -h now");
}

void VersionPage::rebootSystem()
{
    system("reboot");
}

void VersionPage::updatePackage()
{
    UpdateDialog updateDialog;

    updateDialog.exec();
}

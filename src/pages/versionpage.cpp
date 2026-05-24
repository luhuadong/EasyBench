#include "versionpage.h"
#include "tb_common.h"
#include <QTextCodec>
#include <QStringList>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QVBoxLayout>
#include <QDebug>

#define TAB_ITEM_HEIGHT 48
constexpr int kVersionLabelColumnWidth = 180;
constexpr int kVersionTableMinWidth = 560;

VersionPage::VersionPage(TbOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("Version Information"));
    setTitleLabelText(tr("设备版本信息"));

    QWidget *content = contentArea();

    tabView = new QTableView(content);
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
    tabView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tabView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //tabView->horizontalHeader()->setSectionsClickable(false);    // 设置水平表头不可点击
    tabView->setSelectionBehavior(QAbstractItemView::SelectRows);    // 设置每次点击选中一整行
    tabView->setSelectionMode(QAbstractItemView::SingleSelection);   // 设置每次只能选中一行
    tabView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置为不可编辑
    tabView->setShowGrid(true);
    tabView->setAlternatingRowColors(true);
//    tabView->setFrameShape(QFrame::Box);
    QFont font = tabView->horizontalHeader()->font();    // 设置水平表头字体加粗
    font.setBold(true);
    tabView->horizontalHeader()->setFont(font);
    tabView->verticalHeader()->setDefaultSectionSize(TAB_ITEM_HEIGHT);
    tabView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    tabView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tabView->setColumnWidth(0, kVersionLabelColumnWidth);

    getVersionData();

    const int tableHeight =
        tabModel->rowCount() * TAB_ITEM_HEIGHT + tabView->frameWidth() * 2;
    tabView->setFixedHeight(tableHeight);
    tabView->setMinimumWidth(kVersionTableMinWidth);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(80, 12, 80, 12);
    pageLayout->addStretch();
    pageLayout->addWidget(tabView, 0);
    pageLayout->addStretch();
}

void VersionPage::getVersionData()
{
    QStringList itemNameList;
    itemNameList << tr("设备型号") << tr("系统版本") << tr("发行版本") << tr("内核版本") << tr("引导程序")
                 << tr("GCC版本") << tr("硬件型号") << tr("开发者") << tr("TuxiBit");

    QStringList itemValueList;
    itemValueList << g_opt->getProductInfo()
                  << g_opt->getCustomOSInfo()
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

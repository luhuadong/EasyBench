#include "versionpage.h"
#include <QTextCodec>
#include <QStringList>
#include <QFile>

VersionPage::VersionPage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Version Information"));

    //-------------- table ----------------
    tabView = new QTableView(this);
    //tabView->setFixedSize(720, 288);
    //tabView->move(0, 96);
    tabView->setGeometry(0+80, 96+36, 900-160, 720-96-72-72);
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
    tabView->verticalHeader()->setDefaultSectionSize(48);   // 改变默认行高
    //tabView->horizontalHeader()->setTextElideMode(Qt::ElideLeft);    // 超出显示区域时省略号在左边
    tabView->setColumnWidth(0, 150); // 设置第一列宽度为150
    //tabView->setColumnWidth(1, 400); // 设置第二列宽度为400
    tabView->horizontalHeader()->setStretchLastSection(true);    // 设置最后一列充满表宽度

    //-----------------------------
    getVersionData();


}

void VersionPage::getVersionData()
{
    QString kernelStr;
    QFile tmpFile;
    tmpFile.setFileName("/proc/version");
    if(!tmpFile.open(QIODevice::ReadOnly)) {
        kernelStr = tr("3.10.17");
    }
    else {
        kernelStr = tmpFile.readLine();
    }
    tmpFile.close();

    QStringList itemNameList;
    itemNameList << tr("Machine model") << tr("Serial number") << tr("GYT OS version")
                 << tr("Yocto version") << tr("Linux kernel") << tr("Firmware version")
                 << tr("Software version") << tr("Hardware version") << tr("Developer") << tr(" ");

    QStringList itemValueList;
    itemValueList << tr("GY64") << tr("xxxx-xxxx-xxxx-xxxx") << tr("1.0.001")
                  << tr("1.5.3") << kernelStr << tr("fs 1.0.0-B2016112801")
                  << tr("seat_imx 2.0.0.10554") << tr("Freescale i.MX6 Quad, ARMv7 Processor rev 10")
                  << tr("Guangzhou Guangyou communications equipment Co., Ltd.")
                  << tr(" ");

    for (int row = 0; row < 10; ++row) {
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

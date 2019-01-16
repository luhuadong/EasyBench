#include "versionpage.h"
#include "gyt_common.h"
#include <QTextCodec>
#include <QStringList>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>

VersionPage::VersionPage(QWidget *parent) :
    PageWidget(parent)
{
    //setTitleLabelText(tr("Version Information"));
    setTitleLabelText(tr("设备版本信息"));

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


#if LANGUAGE_CHINESE
    operationBar->firstButton()->setText(tr("关机"));
    operationBar->secondButton()->setText(tr("重启"));
#else
    operationBar->firstButton()->setText(tr("Shutdown"));
    operationBar->secondButton()->setText(tr("Reboot"));
#endif
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->firstButton(), SIGNAL(clicked()), this, SLOT(shutdownSystem()));
    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(rebootSystem()));


    getVersionData();


}

void VersionPage::getVersionData()
{
    gyVersionFile = QString("/etc/gy-version");
    QSettings gyVersionRead(gyVersionFile, QSettings::IniFormat);
    QString v_productName = gyVersionRead.value("/PLATFORM/Product", QString("GY33ASEAT")).toString();
    QString v_serialNumber = gyVersionRead.value("/PLATFORM/SN", QString("4401-2518-9763-AC08")).toString();
    QString v_gyos = gyVersionRead.value("/LINUX/GYOS", QString("GYLinux_GW_SV_1.0.1")).toString();
    QString v_yocto = gyVersionRead.value("/LINUX/YOCTO", QString("Freescale i.MX Release Distro Yocto 1.8")).toString();
    QString v_kernel = gyVersionRead.value("/LINUX/Kernel", QString("3.14.52")).toString();
    QString v_uboot = gyVersionRead.value("/LINUX/UBoot", QString("2015.04-g624b022")).toString();
    QString v_gcc = gyVersionRead.value("/LINUX/GCC", QString("arm-poky-linux-gnueabi-gcc")).toString();
    QString v_model = gyVersionRead.value("/PLATFORM/Vendor", QString("GYT")).toString() + QString(" ")
                    + gyVersionRead.value("/PLATFORM/Model", QString("Advantech ROM-5420-B1")).toString();
    QString v_baseBoard = gyVersionRead.value("/PLATFORM/BaseBoard", QString("C019 v1.3")).toString();
    QString v_developer = gyVersionRead.value("/PLATFORM/Developer", QString("广州广有通信设备有限公司")).toString();


    QStringList itemNameList;
    itemNameList << tr("设备型号") << tr("序列号") << tr("系统版本")
                 << tr("Yocto版本") << tr("内核版本") << tr("Uboot版本")
                 << tr("GCC版本") << tr("核心模块") << tr("硬件版本") << tr("开发者");

    QStringList itemValueList;
    itemValueList << v_productName << v_serialNumber << v_gyos
                  << v_yocto << v_kernel << v_uboot
                  << v_gcc << v_model << v_baseBoard << v_developer;

#if 0
    QString kernelStr;
    QString yoctoStr;
    QFile tmpFile;

    tmpFile.setFileName("/proc/version");
    if(!tmpFile.open(QIODevice::ReadOnly)) {
        kernelStr = tr("3.10.17");
    }
    else {
        kernelStr = tmpFile.readLine();
    }
    tmpFile.close();

    tmpFile.setFileName("/etc/issue.net");
    if(!tmpFile.open(QIODevice::ReadOnly)) {
        yoctoStr = tr("Yocto 1.8");
    }
    else {
        yoctoStr = tmpFile.readLine();
    }
    tmpFile.close();


    QStringList itemNameList;
    itemNameList << tr("设备型号") << tr("序列号") << tr("系统版本")
                 << tr("Yocto版本") << tr("内核版本") << tr("固件版本")
                 << tr("软件版本") << tr("硬件版本") << tr("开发者") << tr(" ");

    QStringList itemValueList;
    itemValueList << QString(gMachineTypeStr) << tr("4401-2518-9763-AC08") << tr("GYLinux_GW_SV_1.0.1")
                  << yoctoStr << kernelStr << tr("fs 1.0.0-B2016112801")
                  << tr("seat_imx 2.0.0.10554") << tr("Freescale i.MX6 Quad, ARMv7 Processor rev 10")
                  << tr("Guangzhou Guangyou communications equipment Co., Ltd.")
                  << tr(" ");
#endif

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

#include "monitorpage.h"
#include "gyt_common.h"
#include "module/monitor/basepcbthread.h"
#include "module/monitor/cpustatthread.h"
#include <QTextCodec>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>

extern "C"
{
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/vfs.h>
}

//unsigned char calcCheckSum(unsigned char *data, unsigned int len)
//{
//    unsigned char sum = 0x00;
//    while(len--) {
//        sum += data[len];
//    }
//    return sum;
//}

MonitorPage::MonitorPage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("System Monitor"));
    setTitleLabelText(tr("系统性能监测"));

    totalCpuNum = sysconf(_SC_NPROCESSORS_CONF);
    availCpuNum = sysconf(_SC_NPROCESSORS_ONLN);

    updateSysParam();

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setGeometry(0, 96, this->width(), this->height()-96-72);

    // -----------------------------------
    cpuGroupBox = new QGroupBox(tr("处理器"), mainWidget);
    cpuNameLabel = new QLabel(tr("ARMv7 Processor rev 10 (v7l)"), cpuGroupBox);
    cpuVendorLabel = new QLabel(tr("Freescale i.MX6 Quad/DualLite (Device Tree)"), cpuGroupBox);
    cpuCoreLabel = new QLabel(QString("CPU core : %1 (total), %2 (available)").arg(totalCpuNum).arg(availCpuNum), cpuGroupBox);
    cpuTotalDutyLabel = new QLabel(tr("cpu total duty : %1 %").arg(cpuTotalDuty, 0, 'f', 1), cpuGroupBox);
    cpuBar = new QProgressBar(cpuGroupBox);
    cpuBar->setMinimum(0);
    cpuBar->setMaximum(100);

    QVBoxLayout *cpuGroupLayout = new QVBoxLayout;
    cpuGroupLayout->addWidget(cpuNameLabel);
    cpuGroupLayout->addWidget(cpuVendorLabel);
    cpuGroupLayout->addWidget(cpuCoreLabel);
    cpuGroupLayout->addWidget(cpuTotalDutyLabel);
    cpuGroupLayout->addWidget(cpuBar);

    cpuGroupBox->setLayout(cpuGroupLayout);

    // -----------------------------------
    tempGroupBox = new QGroupBox(tr("温度"), mainWidget);

    armTempLabel = new QLabel(QString("ARM temp  : %1 C").arg(armTemp, 0, 'f', 2), tempGroupBox);
    adspTempLabel = new QLabel(QString("ADSP temp : %1 C").arg(adspTemp, 0, 'f', 2), tempGroupBox);
    pcbTempLabel = new QLabel(QString("PCB temp  : %1 C").arg(pcbTemp, 0, 'f', 2), tempGroupBox);
    fanSpeedLabel = new QLabel(QString("Fan speed : %1 rpm").arg(fanSpeed, 0, 'f', 0), tempGroupBox);
    fwVerLabel = new QLabel(tr("firmware : "), tempGroupBox);
    hwVerLabel = new QLabel(tr("hardware : "), tempGroupBox);

    QVBoxLayout *tempGroupLayout = new QVBoxLayout;
    tempGroupLayout->addWidget(armTempLabel);
    tempGroupLayout->addWidget(adspTempLabel);
    tempGroupLayout->addWidget(pcbTempLabel);
    tempGroupLayout->addWidget(fanSpeedLabel);
    tempGroupLayout->addWidget(fwVerLabel);
    tempGroupLayout->addWidget(hwVerLabel);

    tempGroupBox->setLayout(tempGroupLayout);

    // -----------------------------------
    memGroupBox = new QGroupBox(tr("内存"), mainWidget);

    memTotalLabel = new QLabel(QString("Total : %1 MB").arg(memTotal), memGroupBox);
    memUsedLabel = new QLabel(QString("Used  : %1 MB").arg(memUsed), memGroupBox);
    memFreeLabel = new QLabel(QString("Free  : %1 MB").arg(memFree), memGroupBox);
    memBar = new QProgressBar(memGroupBox);
    memBar->setMinimum(0);
    memBar->setMaximum(memTotal);
    memBar->setValue(memUsed);

    QHBoxLayout *memInfoLayout = new QHBoxLayout;
    memInfoLayout->addWidget(memTotalLabel);
    memInfoLayout->addWidget(memUsedLabel);
    memInfoLayout->addWidget(memFreeLabel);
    QVBoxLayout *memGroupLayout = new QVBoxLayout;
    memGroupLayout->addLayout(memInfoLayout);
    memGroupLayout->addWidget(memBar);

    memGroupBox->setLayout(memGroupLayout);

    // -----------------------------------
    diskGroupBox = new QGroupBox(tr("硬盘"), mainWidget);

    diskTotalLabel = new QLabel(QString("Total : %1 MB").arg(diskTotal), diskGroupBox);
    diskUsedLabel = new QLabel(QString("Used  : %1 MB").arg(diskUsed), diskGroupBox);
    diskFreeLabel = new QLabel(QString("Free  : %1 MB").arg(diskFree), diskGroupBox);
    diskBar = new QProgressBar(diskGroupBox);
    diskBar->setMinimum(0);
    diskBar->setMaximum(diskTotal);
    diskBar->setValue(diskUsed);

    QHBoxLayout *diskInfoLayout = new QHBoxLayout;
    diskInfoLayout->addWidget(diskTotalLabel);
    diskInfoLayout->addWidget(diskUsedLabel);
    diskInfoLayout->addWidget(diskFreeLabel);
    QVBoxLayout *diskGroupLayout = new QVBoxLayout;
    diskGroupLayout->addLayout(diskInfoLayout);
    diskGroupLayout->addWidget(diskBar);

    diskGroupBox->setLayout(diskGroupLayout);

    // -----------------------------------

    QVBoxLayout *rightBottomLayout = new QVBoxLayout;
    rightBottomLayout->setSpacing(10);
    rightBottomLayout->setMargin(0);
    rightBottomLayout->addWidget(memGroupBox);
    rightBottomLayout->addWidget(diskGroupBox);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(tempGroupBox, 1);
    bottomLayout->addLayout(rightBottomLayout, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    mainLayout->addWidget(cpuGroupBox);
    mainLayout->addLayout(bottomLayout);

    mainWidget->setLayout(mainLayout);


    fanMode = 0;
    operationBar->firstButton()->setEnabled(false);
    operationBar->secondButton()->setText(tr("Standard"));
    operationBar->thirdButton()->setText(tr("Change"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);
    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(fanModeBtnClicked()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(changeFanSpeedBtnClicked()));

#if CONNECT_STM32
    createSocketWithBasePcb();
    if(sockToBasdPcbIsOk) {
        BasePcbThread *pcbThread = new BasePcbThread(sockfd, this);
        pcbThread->start();

        sendBasePcbCmd(CMD_GET_VERSION, QString(""));
        sendBasePcbCmd(CMD_CONTROL, QString(""));
    }


    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(on_updateTimer_timeout()));
    updateTimer->start(1000);

    CpuStatThread *cpuStatThread = new CpuStatThread(this);
    cpuStatThread->start();
#endif
}

void MonitorPage::createSocketWithBasePcb()
{
    struct sockaddr_in dest;
    //char buffer[MAXBUF + 1];

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        //exit(EXIT_FAILURE);
        return ;
    }
    printf("sockfd = %d\n", sockfd);

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8080);

    if(inet_aton("192.168.3.188", (struct in_addr *)&dest.sin_addr.s_addr) == 0)
    {
        //perror(argv[1]);
        //exit(errno);
        return ;
    }

    if(::connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) == -1)
    {
        perror("connect");
        sockToBasdPcbIsOk = false;
        //exit(errno);
        return ;
    }
    sockToBasdPcbIsOk = true;
    //printf("server connected\n");
}

void MonitorPage::sendBasePcbCmd(BASEPCB_CMD which, const QString & arg)
{
    if(!sockToBasdPcbIsOk) return;

    unsigned char buffer[MAXBUF + 1];
    int len;
    bzero(buffer, MAXBUF + 1);

    if(which == CMD_GET_STATUS)
    {
        //printf("I want to get device status.\n");

        applyCMD cmd = {0xAA, 0x07, 0xA0, 0x00,\
                        0xA0, 0x00, 0xEE};

        cmd.csum = calcCheckSum((unsigned char *)&cmd, sizeof(cmd)-2);
        //printf("cmd.sum = %02x\n", cmd.csum);

        memcpy(buffer, &cmd, sizeof(applyCMD));

        len = send(sockfd, buffer, buffer[1], 0);
        if(len < 0)
        {
            printf("message '%s' send failed! errno code is %d, errno message is '%s'\n", buffer, errno, strerror(errno));
            return ;
        }
        //printf("len = %d\n", len);
    }
    else if(which == CMD_GET_VERSION)
    {
        printf("I want to get version information.\n");

        applyCMD cmd = {0xAA, 0x07, 0xA0, 0x00,\
                        0xA1, 0x00, 0xEE};

        cmd.csum = calcCheckSum((unsigned char *)&cmd, sizeof(cmd)-2);
        printf("cmd.sum = %02x\n", cmd.csum);

        memcpy(buffer, &cmd, sizeof(applyCMD));

        len = send(sockfd, buffer, buffer[1], 0);
        if(len < 0) {
            printf("message '%s' send failed! errno code is %d, errno message is '%s'\n", buffer, errno, strerror(errno));
            return ;
        }
        //printf("len = %d\n", len);

    }
    else if(which == CMD_CONTROL)
    {
        printf("I want to control Fan speed.\n");
        controlCMD cmd = {0x00};
        cmd.head = 0xAA;
        cmd.size = sizeof(cmd);
        cmd.type = 0xA0;
        cmd.snum = 0x00;
        cmd.cmdw = 0xA2;
        cmd.mode = arg.isEmpty() ? 0x00 : 0x01;
        cmd.fanSpeed = arg.isEmpty() ? 0.0 : arg.toFloat();
        cmd.csum = calcCheckSum((unsigned char *)&cmd, sizeof(cmd)-2);
        cmd.tail = 0xEE;

        //printf("cmd.sum = %02x\n", cmd.csum);

        memcpy(buffer, &cmd, sizeof(controlCMD));

        len = send(sockfd, buffer, buffer[1], 0);
        if(len < 0) {
            printf("message '%s' send failed! errno code is %d, errno message is '%s'\n", buffer, errno, strerror(errno));
            return ;
        }
        //printf("len = %d\n", len);
    }
    else
    {
        printf("no cmd\n");
    }
}

void MonitorPage::on_updateTimer_timeout()
{
    sendBasePcbCmd(CMD_GET_STATUS, QString(""));
    updateSysParam();

    cpuTotalDutyLabel->setText(tr("cpu total duty : %1 %").arg(cpuTotalDuty, 0, 'f', 1));
    cpuBar->setValue(cpuTotalDuty);

    //memTotalLabel->setText(QString("Total : %1 MB").arg(memTotal));
    memUsedLabel->setText(QString("Used  : %1 MB").arg(memUsed));
    memFreeLabel->setText(QString("Free  : %1 MB").arg(memFree));
    memBar->setValue(memUsed);

    //diskTotalLabel->setText(QString("Total : %1 MB").arg(diskTotal));
    diskUsedLabel->setText(QString("Used  : %1 MB").arg(diskUsed));
    diskFreeLabel->setText(QString("Free  : %1 MB").arg(diskFree));
    diskBar->setValue(diskUsed);

    armTempLabel->setText(QString("ARM temp  : %1 C").arg(armTemp, 0, 'f', 2));

    if(sockToBasdPcbIsOk) {

        adspTempLabel->setText(QString("ADSP temp : %1 C").arg(adspTemp, 0, 'f', 2));
        pcbTempLabel->setText(QString("PCB temp  : %1 C").arg(pcbTemp, 0, 'f', 2));
        fanSpeedLabel->setText(QString("Fan speed : %1 rpm").arg(fanSpeed, 0, 'f', 0));
    }
}

void MonitorPage::updateSysParam()
{
    QString tmpStr;
    QStringList tmpStrList;
    QFile tmpFile;
    //int pos;

    //-------------------------------

    tmpFile.setFileName(TEMP_FILE);
    if(!tmpFile.open(QIODevice::ReadOnly)) {
        tmpStr = tr("none");
    }
    else {
        tmpStr = tmpFile.readLine();
        tmpStr = tmpStr.left(tmpStr.length() - 1);
        tmpStr.insert(2, QChar('.'));
        armTemp = tmpStr.toFloat();
    }
    tmpFile.close();

    //-------------------------------

    tmpFile.setFileName(MEM_FILE);
    if(tmpFile.open(QIODevice::ReadOnly)) {
        while(1) {
            tmpStr = tmpFile.readLine();

            if(-1 != tmpStr.indexOf("MemTotal")) {
                tmpStrList = tmpStr.split(QRegExp("\\s+"));
                memTotal = tmpStrList.at(1).toUInt() / 1024;
                continue;
            }
            else if(-1 != tmpStr.indexOf("MemFree")) {
                tmpStrList = tmpStr.split(QRegExp("\\s+"));
                memFree = tmpStrList.at(1).toUInt() / 1024;
                break;
            }
        }
        memUsed = memTotal - memFree;
    }
    tmpFile.close();

    //-------------------------------

    struct statfs diskInfo;
    statfs("/", &diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
    diskTotal = totalSize>>20; // MB
    unsigned long long freeDisk = diskInfo.f_bfree * totalBlocks;
    diskFree = freeDisk>>20; // MB
    diskUsed = diskTotal - diskFree;

}

void MonitorPage::fanModeBtnClicked(void)
{
    if(!sockToBasdPcbIsOk) return;

    if(fanMode == 0) {
        fanMode = 1;
        //sendBasePcbCmd(CMD_CONTROL, QString(""));
        operationBar->secondButton()->setText(tr("Debug"));
        operationBar->thirdButton()->setEnabled(true);
    }
    else if(fanMode == 1) {
        fanMode = 0;
        sendBasePcbCmd(CMD_CONTROL, QString(""));
        operationBar->secondButton()->setText(tr("Standard"));
        operationBar->thirdButton()->setEnabled(false);
    }
}

void MonitorPage::changeFanSpeedBtnClicked(void)
{
    static int down = 0;
    if(!sockToBasdPcbIsOk) return;

    if(down == 0) {
        sendBasePcbCmd(CMD_CONTROL, QString("1500.00"));
        down = 1;
    }
    else if(down == 1) {
        sendBasePcbCmd(CMD_CONTROL, QString("2500.00"));
        down = 2;
    }
    else if(down == 2) {
        sendBasePcbCmd(CMD_CONTROL, QString("3500.00"));
        down = 3;
    }
    else if(down == 3) {
        sendBasePcbCmd(CMD_CONTROL, QString("4500.00"));
        down = 0;
    }
}

void MonitorPage::setBasePcbTemp1Text(const float arg)
{
    adspTemp = arg;
}

void MonitorPage::setBasePcbTemp2Text(const float arg)
{
    pcbTemp = arg;
}

void MonitorPage::setFanSpeedText(const float arg)
{
    fanSpeed = arg;
}

void MonitorPage::setFwVerText(const QString text)
{
    fwVerLabel->setText(text);
}

void MonitorPage::setHwVerText(const QString text)
{
    hwVerLabel->setText(text);
}

void MonitorPage::setCpuDuty(const float arg)
{
    cpuTotalDuty = arg;
}

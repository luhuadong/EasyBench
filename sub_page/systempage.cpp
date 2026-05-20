#include "systempage.h"
#include "module/monitor/basepcbthread.h"
#include "module/monitor/cpustatthread.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>

extern "C" {
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
}

SystemPage::SystemPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("系统信息"));
    buildUi();
    initOperationBar();
    updateSysParam();

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &SystemPage::onUpdateTimer);
    updateTimer->start(1000);

    CpuStatThread *cpuStatThread = new CpuStatThread(this);
    cpuStatThread->start();

#if CONNECT_STM32
    createSocketWithBasePcb();
    if (sockToBasdPcbIsOk) {
        BasePcbThread *pcbThread = new BasePcbThread(sockfd, this);
        pcbThread->start();
        sendBasePcbCmd(CMD_GET_VERSION, QString());
        sendBasePcbCmd(CMD_CONTROL, QString());
    }
    operationBar->secondButton()->setText(tr("Standard"));
    operationBar->thirdButton()->setText(tr("Change"));
    operationBar->thirdButton()->setEnabled(false);
    connect(operationBar->secondButton(), &QPushButton::clicked, this, &SystemPage::fanModeBtnClicked);
    connect(operationBar->thirdButton(), &QPushButton::clicked, this, &SystemPage::changeFanSpeedBtnClicked);
#else
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(false);
#endif
    operationBar->fourthButton()->setEnabled(false);
}

void SystemPage::buildUi()
{
    QWidget *content = contentArea();

    cpuGroup = new QGroupBox(tr("处理器"), content);
    cpuNameLabel = new QLabel(cpuGroup);
    cpuNameLabel->setWordWrap(true);
    cpuVendorLabel = new QLabel(cpuGroup);
    cpuVendorLabel->setWordWrap(true);
    cpuCoreLabel = new QLabel(cpuGroup);
    cpuDutyLabel = new QLabel(cpuGroup);
    cpuBar = new QProgressBar(cpuGroup);
    cpuBar->setRange(0, 100);

    QVBoxLayout *cpuLayout = new QVBoxLayout(cpuGroup);
    cpuLayout->setContentsMargins(12, 16, 12, 12);
    cpuLayout->addWidget(cpuNameLabel);
    cpuLayout->addWidget(cpuVendorLabel);
    cpuLayout->addWidget(cpuCoreLabel);
    cpuLayout->addWidget(cpuDutyLabel);
    cpuLayout->addWidget(cpuBar);

    memGroup = new QGroupBox(tr("内存"), content);
    memTotalLabel = new QLabel(memGroup);
    memUsedLabel = new QLabel(memGroup);
    memFreeLabel = new QLabel(memGroup);
    memBar = new QProgressBar(memGroup);
    memBar->setRange(0, 100);

    QVBoxLayout *memLayout = new QVBoxLayout(memGroup);
    memLayout->setContentsMargins(12, 16, 12, 12);
    memLayout->addWidget(memTotalLabel);
    memLayout->addWidget(memUsedLabel);
    memLayout->addWidget(memFreeLabel);
    memLayout->addWidget(memBar);

    diskGroup = new QGroupBox(tr("存储"), content);
    diskTotalLabel = new QLabel(diskGroup);
    diskUsedLabel = new QLabel(diskGroup);
    diskFreeLabel = new QLabel(diskGroup);
    diskBar = new QProgressBar(diskGroup);
    diskBar->setRange(0, 100);

    QVBoxLayout *diskLayout = new QVBoxLayout(diskGroup);
    diskLayout->setContentsMargins(12, 16, 12, 12);
    diskLayout->addWidget(diskTotalLabel);
    diskLayout->addWidget(diskUsedLabel);
    diskLayout->addWidget(diskFreeLabel);
    diskLayout->addWidget(diskBar);

    rtcGroup = new QGroupBox(tr("实时时钟"), content);
    rtcNameLabel = new QLabel(tr("设备型号 : —"), rtcGroup);
    rtcDateTimeLabel = new QLabel(tr("RTC 时间 : —"), rtcGroup);
    sysDateTimeLabel = new QLabel(tr("系统时间 : —"), rtcGroup);
    QVBoxLayout *rtcLayout = new QVBoxLayout(rtcGroup);
    rtcLayout->setContentsMargins(12, 16, 12, 12);
    rtcLayout->addWidget(rtcNameLabel);
    rtcLayout->addWidget(rtcDateTimeLabel);
    rtcLayout->addWidget(sysDateTimeLabel);

    tempGroup = new QGroupBox(tr("温度 / 底板"), content);
    armTempLabel = new QLabel(tempGroup);
    adspTempLabel = new QLabel(tempGroup);
    pcbTempLabel = new QLabel(tempGroup);
    fanSpeedLabel = new QLabel(tempGroup);
    fwVerLabel = new QLabel(tr("固件 : —"), tempGroup);
    hwVerLabel = new QLabel(tr("硬件 : —"), tempGroup);
    QVBoxLayout *tempLayout = new QVBoxLayout(tempGroup);
    tempLayout->setContentsMargins(12, 16, 12, 12);
    tempLayout->addWidget(armTempLabel);
    tempLayout->addWidget(adspTempLabel);
    tempLayout->addWidget(pcbTempLabel);
    tempLayout->addWidget(fanSpeedLabel);
    tempLayout->addWidget(fwVerLabel);
    tempLayout->addWidget(hwVerLabel);

    QGridLayout *grid = new QGridLayout(content);
    grid->setContentsMargins(16, 12, 16, 12);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(12);
    grid->addWidget(cpuGroup, 0, 0, 1, 2);
    grid->addWidget(memGroup, 1, 0);
    grid->addWidget(diskGroup, 1, 1);
    grid->addWidget(rtcGroup, 2, 0);
    grid->addWidget(tempGroup, 2, 1);
}

void SystemPage::initOperationBar()
{
    if (system("netstat -nult | grep -w 22") == 0) {
        operationBar->firstButton()->setText(tr("SSH已开启"));
    } else {
        operationBar->firstButton()->setText(tr("SSH已关闭"));
    }
    connect(operationBar->firstButton(), &QPushButton::clicked, this, &SystemPage::onSshBtnClicked);
}

void SystemPage::updateSysParam()
{
    EbSysStats::readCpuInfo(&cpuInfo);
    EbSysStats::readMemInfo(&memInfo);
    EbSysStats::readDiskInfo(QStringLiteral("/"), &diskInfo);
    EbSysStats::readArmTemperature(&armTemp);

    cpuNameLabel->setText(tr("型号：%1").arg(cpuInfo.modelName));
    cpuVendorLabel->setText(tr("平台：%1").arg(cpuInfo.hardware.isEmpty() ? tr("—") : cpuInfo.hardware));
    cpuCoreLabel->setText(tr("核心：共 %1，在线 %2")
                             .arg(cpuInfo.totalCores)
                             .arg(cpuInfo.onlineCores));

    memTotalLabel->setText(tr("总量：%1 MB").arg(memInfo.totalMb));
    memUsedLabel->setText(tr("已用：%1 MB").arg(memInfo.usedMb));
    memFreeLabel->setText(tr("可用：%1 MB").arg(memInfo.freeMb));
    memBar->setMaximum(static_cast<int>(memInfo.totalMb > 0 ? memInfo.totalMb : 100));
    memBar->setValue(static_cast<int>(memInfo.usedMb));

    diskTotalLabel->setText(tr("根分区总量：%1 MB").arg(diskInfo.totalMb));
    diskUsedLabel->setText(tr("已用：%1 MB").arg(diskInfo.usedMb));
    diskFreeLabel->setText(tr("可用：%1 MB").arg(diskInfo.freeMb));
    diskBar->setMaximum(static_cast<int>(diskInfo.totalMb > 0 ? diskInfo.totalMb : 100));
    diskBar->setValue(static_cast<int>(diskInfo.usedMb));

    armTempLabel->setText(tr("SoC 温度：%1 °C").arg(armTemp, 0, 'f', 1));

    QFile rtcNameFile(QStringLiteral("/sys/class/rtc/rtc0/name"));
    if (rtcNameFile.open(QIODevice::ReadOnly)) {
        rtcNameLabel->setText(tr("设备型号：%1").arg(QString::fromUtf8(rtcNameFile.readAll()).trimmed()));
    }

    const QString rtcTime = EbSysStats::readRtcDateTime();
    if (!rtcTime.isEmpty()) {
        rtcDateTimeLabel->setText(tr("RTC 时间：%1").arg(rtcTime));
    } else {
        rtcDateTimeLabel->setText(tr("RTC 时间：不可用（无硬件 RTC 或权限不足）"));
    }

    FILE *dateStream = popen("date 2>/dev/null", "r");
    if (dateStream) {
        char buf[128] = {0};
        if (fgets(buf, sizeof(buf), dateStream)) {
            sysDateTimeLabel->setText(tr("系统时间：%1").arg(QString::fromUtf8(buf).trimmed()));
        }
        pclose(dateStream);
    }
}

void SystemPage::onUpdateTimer()
{
#if CONNECT_STM32
    if (sockToBasdPcbIsOk) {
        sendBasePcbCmd(CMD_GET_STATUS, QString());
    }
#endif
    updateSysParam();

    cpuDutyLabel->setText(tr("总占用率：%1 %").arg(cpuTotalDuty, 0, 'f', 1));
    cpuBar->setValue(static_cast<int>(cpuTotalDuty));

    if (sockToBasdPcbIsOk) {
        adspTempLabel->setText(tr("ADSP 温度：%1 °C").arg(adspTemp, 0, 'f', 1));
        pcbTempLabel->setText(tr("PCB 温度：%1 °C").arg(pcbTemp, 0, 'f', 1));
        fanSpeedLabel->setText(tr("风扇转速：%1 rpm").arg(fanSpeed, 0, 'f', 0));
    } else {
        adspTempLabel->setText(tr("ADSP 温度：— (未连接底板)"));
        pcbTempLabel->setText(tr("PCB 温度：—"));
        fanSpeedLabel->setText(tr("风扇转速：—"));
    }
}

void SystemPage::onSshBtnClicked()
{
    if (operationBar->firstButton()->text() == tr("SSH已关闭")) {
        system("/etc/init.d/sshd start");
        operationBar->firstButton()->setText(tr("SSH已开启"));
    } else if (operationBar->firstButton()->text() == tr("SSH已开启")) {
        system("/etc/init.d/sshd stop");
        operationBar->firstButton()->setText(tr("SSH已关闭"));
    }
}

void SystemPage::setCpuDuty(float value)
{
    cpuTotalDuty = value;
}

void SystemPage::setBasePcbTemp1Text(float value)
{
    adspTemp = value;
}

void SystemPage::setBasePcbTemp2Text(float value)
{
    pcbTemp = value;
}

void SystemPage::setFanSpeedText(float value)
{
    fanSpeed = value;
}

void SystemPage::setFwVerText(const QString &text)
{
    fwVerLabel->setText(text);
}

void SystemPage::setHwVerText(const QString &text)
{
    hwVerLabel->setText(text);
}

void SystemPage::createSocketWithBasePcb()
{
    struct sockaddr_in dest;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        sockToBasdPcbIsOk = false;
        return;
    }
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8080);
    if (inet_aton("192.168.3.188", &dest.sin_addr) == 0) {
        sockToBasdPcbIsOk = false;
        return;
    }
    if (::connect(sockfd, reinterpret_cast<struct sockaddr *>(&dest), sizeof(dest)) == -1) {
        sockToBasdPcbIsOk = false;
        return;
    }
    sockToBasdPcbIsOk = true;
}

void SystemPage::sendBasePcbCmd(BASEPCB_CMD which, const QString &arg)
{
    if (!sockToBasdPcbIsOk) {
        return;
    }

    unsigned char buffer[MAXBUF + 1];
    memset(buffer, 0, sizeof(buffer));

    if (which == CMD_GET_STATUS) {
        applyCMD cmd = {0xAA, 0x07, 0xA0, 0x00, 0xA0, 0x00, 0xEE};
        cmd.csum = calcCheckSum(reinterpret_cast<unsigned char *>(&cmd), sizeof(cmd) - 2);
        memcpy(buffer, &cmd, sizeof(applyCMD));
        send(sockfd, buffer, buffer[1], 0);
    } else if (which == CMD_GET_VERSION) {
        applyCMD cmd = {0xAA, 0x07, 0xA0, 0x00, 0xA1, 0x00, 0xEE};
        cmd.csum = calcCheckSum(reinterpret_cast<unsigned char *>(&cmd), sizeof(cmd) - 2);
        memcpy(buffer, &cmd, sizeof(applyCMD));
        send(sockfd, buffer, buffer[1], 0);
    } else if (which == CMD_CONTROL) {
        controlCMD cmd = {};
        cmd.head = 0xAA;
        cmd.size = sizeof(cmd);
        cmd.type = 0xA0;
        cmd.snum = 0x00;
        cmd.cmdw = 0xA2;
        cmd.mode = arg.isEmpty() ? 0x00 : 0x01;
        cmd.fanSpeed = arg.isEmpty() ? 0.0f : arg.toFloat();
        cmd.csum = calcCheckSum(reinterpret_cast<unsigned char *>(&cmd), sizeof(cmd) - 2);
        cmd.tail = 0xEE;
        memcpy(buffer, &cmd, sizeof(controlCMD));
        send(sockfd, buffer, buffer[1], 0);
    }
}

void SystemPage::fanModeBtnClicked()
{
    if (!sockToBasdPcbIsOk) {
        return;
    }
    if (fanMode == 0) {
        fanMode = 1;
        operationBar->secondButton()->setText(tr("Debug"));
        operationBar->thirdButton()->setEnabled(true);
    } else {
        fanMode = 0;
        sendBasePcbCmd(CMD_CONTROL, QString());
        operationBar->secondButton()->setText(tr("Standard"));
        operationBar->thirdButton()->setEnabled(false);
    }
}

void SystemPage::changeFanSpeedBtnClicked()
{
    static int step = 0;
    if (!sockToBasdPcbIsOk) {
        return;
    }
    const QString speeds[] = {QStringLiteral("1500.00"), QStringLiteral("2500.00"),
                              QStringLiteral("3500.00"), QStringLiteral("4500.00")};
    sendBasePcbCmd(CMD_CONTROL, speeds[step % 4]);
    step = (step + 1) % 4;
}

#include "systempage.h"
#include "tb_thread_util.h"
#include "modules/monitor/basepcbthread.h"
#include "modules/monitor/cpustatthread.h"

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

namespace {

constexpr qint64 kBytesPerUnit = 1024;

void stretchGroupBox(QGroupBox *box)
{
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/** 按 1024 进制将字节数格式化为 B / KB / MB / GB / TB，自动选取合适单位。 */
QString formatByteSize(qint64 bytes)
{
    if (bytes <= 0) {
        return QStringLiteral("0 B");
    }

    static const QString kUnitSuffix[] = {
        QStringLiteral("B"),
        QStringLiteral("KB"),
        QStringLiteral("MB"),
        QStringLiteral("GB"),
        QStringLiteral("TB"),
    };

    int unitIndex = 0;
    double value = static_cast<double>(bytes);
    while (unitIndex < 4 && value >= static_cast<double>(kBytesPerUnit)) {
        value /= static_cast<double>(kBytesPerUnit);
        ++unitIndex;
    }

    int precision = 0;
    if (unitIndex == 0) {
        precision = 0;
    } else if (unitIndex == 1) {
        precision = value < 100.0 ? 1 : 0;
    } else if (value >= 100.0) {
        precision = 1;
    } else if (value >= 10.0) {
        precision = 1;
    } else {
        precision = 2;
    }

    QString number = QString::number(value, 'f', precision);
    if (unitIndex > 0) {
        while (number.contains(QLatin1Char('.')) && number.endsWith(QLatin1Char('0'))) {
            number.chop(1);
        }
        if (number.endsWith(QLatin1Char('.'))) {
            number.chop(1);
        }
    }

    return QStringLiteral("%1 %2").arg(number, kUnitSuffix[unitIndex]);
}

/** 将 megabytes（与 TbSysStats 一致）格式化为合适单位字符串。 */
QString formatSizeFromMegabytes(quint64 megabytes)
{
    const qint64 bytes = static_cast<qint64>(megabytes) * kBytesPerUnit * kBytesPerUnit;
    return formatByteSize(bytes);
}

} // namespace

SystemPage::SystemPage(TbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("系统信息"));
    buildUi();
    updateSysParam();

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &SystemPage::onUpdateTimer);
    updateTimer->start(1000);

    cpuStatThread = new CpuStatThread(this);
    connect(cpuStatThread, &CpuStatThread::cpuDutyUpdated, this, &SystemPage::setCpuDuty);
    cpuStatThread->start();

#if CONNECT_STM32
    createSocketWithBasePcb();
    if (sockToBasdPcbIsOk) {
        BasePcbThread *pcbThread = new BasePcbThread(sockfd, this);
        TbThread::nameQThread(pcbThread, "eb-base-pcb");
        pcbThread->start();
        sendBasePcbCmd(CMD_GET_VERSION, QString());
        sendBasePcbCmd(CMD_CONTROL, QString());
    }
    if (fanModeBtn && fanChangeBtn) {
        fanModeBtn->setText(tr("Standard"));
        fanChangeBtn->setEnabled(false);
        connect(fanModeBtn, &QPushButton::clicked, this, &SystemPage::fanModeBtnClicked);
        connect(fanChangeBtn, &QPushButton::clicked, this, &SystemPage::changeFanSpeedBtnClicked);
    }
#endif
}

SystemPage::~SystemPage()
{
    if (cpuStatThread && cpuStatThread->isRunning()) {
        cpuStatThread->requestInterruption();
        cpuStatThread->wait(3000);
    }
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
    cpuLayout->addStretch();

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
    memLayout->addStretch();

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
    diskLayout->addStretch();

    rtcGroup = new QGroupBox(tr("实时时钟"), content);
    rtcNameLabel = new QLabel(tr("设备型号：—"), rtcGroup);
    rtcDateTimeLabel = new QLabel(tr("RTC 时间：—"), rtcGroup);
    sysDateTimeLabel = new QLabel(tr("系统时间：—"), rtcGroup);
    QVBoxLayout *rtcLayout = new QVBoxLayout(rtcGroup);
    rtcLayout->setContentsMargins(12, 16, 12, 12);
    rtcLayout->addWidget(rtcNameLabel);
    rtcLayout->addWidget(rtcDateTimeLabel);
    rtcLayout->addWidget(sysDateTimeLabel);
    rtcLayout->addStretch();

    tempGroup = new QGroupBox(tr("温度 / 底板"), content);
    armTempLabel = new QLabel(tempGroup);
    adspTempLabel = new QLabel(tempGroup);
    pcbTempLabel = new QLabel(tempGroup);
    fanSpeedLabel = new QLabel(tempGroup);
    fwVerLabel = new QLabel(tr("固件：—"), tempGroup);
    hwVerLabel = new QLabel(tr("硬件：—"), tempGroup);
    QVBoxLayout *tempLayout = new QVBoxLayout(tempGroup);
    tempLayout->setContentsMargins(12, 16, 12, 12);
    tempLayout->addWidget(armTempLabel);
    tempLayout->addWidget(adspTempLabel);
    tempLayout->addWidget(pcbTempLabel);
    tempLayout->addWidget(fanSpeedLabel);
    tempLayout->addWidget(fwVerLabel);
    tempLayout->addWidget(hwVerLabel);

#if CONNECT_STM32
    fanModeBtn = new QPushButton(tr("Standard"), tempGroup);
    fanModeBtn->setObjectName(QStringLiteral("functionBtn_small"));
    fanChangeBtn = new QPushButton(tr("Change"), tempGroup);
    fanChangeBtn->setObjectName(QStringLiteral("functionBtn_small"));
    fanChangeBtn->setEnabled(false);
    QHBoxLayout *fanBtnRow = new QHBoxLayout;
    fanBtnRow->addWidget(fanModeBtn);
    fanBtnRow->addWidget(fanChangeBtn);
    fanBtnRow->addStretch();
    tempLayout->addLayout(fanBtnRow);
#endif
    tempLayout->addStretch();

    stretchGroupBox(memGroup);
    stretchGroupBox(diskGroup);
    stretchGroupBox(rtcGroup);
    stretchGroupBox(tempGroup);

    QGridLayout *grid = new QGridLayout;
    grid->setContentsMargins(16, 12, 16, 12);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(12);
    grid->addWidget(cpuGroup, 0, 0, 1, 2);
    grid->addWidget(memGroup, 1, 0);
    grid->addWidget(diskGroup, 1, 1);
    grid->addWidget(rtcGroup, 2, 0);
    grid->addWidget(tempGroup, 2, 1);
    grid->setRowStretch(0, 0);
    grid->setRowStretch(1, 1);
    grid->setRowStretch(2, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addLayout(grid, 1);
}

void SystemPage::updateSysParam()
{
    TbSysStats::readCpuInfo(&cpuInfo);
    TbSysStats::readMemInfo(&memInfo);
    TbSysStats::readDiskInfo(QStringLiteral("/"), &diskInfo);
    TbSysStats::readArmTemperature(&armTemp);

    cpuNameLabel->setText(tr("CPU：%1")
                              .arg(cpuInfo.displayCpuName.isEmpty() ? tr("—") : cpuInfo.displayCpuName));
    cpuVendorLabel->setText(tr("GPU：%1")
                                .arg(cpuInfo.displayGpuName.isEmpty() ? tr("—") : cpuInfo.displayGpuName));
    cpuCoreLabel->setText(tr("核心：共 %1，在线 %2")
                             .arg(cpuInfo.totalCores)
                             .arg(cpuInfo.onlineCores));

    memTotalLabel->setText(tr("总量：%1").arg(formatSizeFromMegabytes(memInfo.totalMb)));
    memUsedLabel->setText(tr("已用：%1").arg(formatSizeFromMegabytes(memInfo.usedMb)));
    memFreeLabel->setText(tr("可用：%1").arg(formatSizeFromMegabytes(memInfo.freeMb)));
    memBar->setMaximum(static_cast<int>(memInfo.totalMb > 0 ? memInfo.totalMb : 100));
    memBar->setValue(static_cast<int>(memInfo.usedMb));

    diskTotalLabel->setText(tr("根分区总量：%1").arg(formatSizeFromMegabytes(diskInfo.totalMb)));
    diskUsedLabel->setText(tr("已用：%1").arg(formatSizeFromMegabytes(diskInfo.usedMb)));
    diskFreeLabel->setText(tr("可用：%1").arg(formatSizeFromMegabytes(diskInfo.freeMb)));
    diskBar->setMaximum(static_cast<int>(diskInfo.totalMb > 0 ? diskInfo.totalMb : 100));
    diskBar->setValue(static_cast<int>(diskInfo.usedMb));

    armTempLabel->setText(tr("SoC 温度：%1 °C").arg(armTemp, 0, 'f', 1));

    QFile rtcNameFile(QStringLiteral("/sys/class/rtc/rtc0/name"));
    if (rtcNameFile.open(QIODevice::ReadOnly)) {
        rtcNameLabel->setText(tr("设备型号：%1").arg(QString::fromUtf8(rtcNameFile.readAll()).trimmed()));
    }

    const QString rtcTime = TbSysStats::readRtcDateTime();
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
        fanModeBtn->setText(tr("Debug"));
        fanChangeBtn->setEnabled(true);
    } else {
        fanMode = 0;
        sendBasePcbCmd(CMD_CONTROL, QString());
        fanModeBtn->setText(tr("Standard"));
        fanChangeBtn->setEnabled(false);
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

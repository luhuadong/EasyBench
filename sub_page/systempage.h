#ifndef SYSTEMPAGE_H
#define SYSTEMPAGE_H

#include "custom_widget/pagewidget.h"
#include "eb_common.h"
#include "module/monitor/eb_sysstats.h"

#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>

#define MAXBUF 256

typedef enum {
    CMD_GET_STATUS,
    CMD_GET_VERSION,
    CMD_CONTROL
} BASEPCB_CMD;

class SystemPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SystemPage(EbOptions *options, QWidget *parent = nullptr);

    void setBasePcbTemp1Text(float value);
    void setBasePcbTemp2Text(float value);
    void setFanSpeedText(float value);
    void setFwVerText(const QString &text);
    void setHwVerText(const QString &text);
    void setCpuDuty(float value);

    bool sockToBasdPcbIsOk = false;

private slots:
    void onUpdateTimer();
    void onSshBtnClicked();
    void fanModeBtnClicked();
    void changeFanSpeedBtnClicked();

private:
    void buildUi();
    void initOperationBar();
    void updateSysParam();
    void createSocketWithBasePcb();
    void sendBasePcbCmd(BASEPCB_CMD which, const QString &arg);

    QGroupBox *cpuGroup = nullptr;
    QLabel *cpuNameLabel = nullptr;
    QLabel *cpuVendorLabel = nullptr;
    QLabel *cpuCoreLabel = nullptr;
    QLabel *cpuDutyLabel = nullptr;
    QProgressBar *cpuBar = nullptr;

    QGroupBox *memGroup = nullptr;
    QLabel *memTotalLabel = nullptr;
    QLabel *memUsedLabel = nullptr;
    QLabel *memFreeLabel = nullptr;
    QProgressBar *memBar = nullptr;

    QGroupBox *diskGroup = nullptr;
    QLabel *diskTotalLabel = nullptr;
    QLabel *diskUsedLabel = nullptr;
    QLabel *diskFreeLabel = nullptr;
    QProgressBar *diskBar = nullptr;

    QGroupBox *rtcGroup = nullptr;
    QLabel *rtcNameLabel = nullptr;
    QLabel *rtcDateTimeLabel = nullptr;
    QLabel *sysDateTimeLabel = nullptr;

    QGroupBox *tempGroup = nullptr;
    QLabel *armTempLabel = nullptr;
    QLabel *adspTempLabel = nullptr;
    QLabel *pcbTempLabel = nullptr;
    QLabel *fanSpeedLabel = nullptr;
    QLabel *fwVerLabel = nullptr;
    QLabel *hwVerLabel = nullptr;

    EbSysStats::CpuInfo cpuInfo;
    float cpuTotalDuty = 0.0f;
    float armTemp = 0.0f;
    float adspTemp = 0.0f;
    float pcbTemp = 0.0f;
    float fanSpeed = 0.0f;
    EbSysStats::MemInfo memInfo;
    EbSysStats::DiskInfo diskInfo;

    QTimer *updateTimer = nullptr;
    int sockfd = -1;
    int fanMode = 0;
};

#endif /* SYSTEMPAGE_H */

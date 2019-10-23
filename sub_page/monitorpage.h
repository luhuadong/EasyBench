#ifndef MONITORPAGE_H
#define MONITORPAGE_H

#include "custom_widget/pagewidget.h"
#include "gyt_common.h"

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
//#include <QProgressBar>
#include <QGroupBox>
#include <QProgressBar>

#define MAXBUF 256

#define TEMP_FILE "/sys/devices/virtual/thermal/thermal_zone0/temp"
//#define TEMP_FILE "/sys/class/thermal/thermal_zone0/temp"
#define CPUINFO_FILE  "/proc/cpuinfo"
#define CPU_FILE           "/proc/stat"
#define MEM_FILE         "/proc/meminfo"
#define DISK_FILE          "/proc/diskstats"
#define PARTITION_FILE  "/proc/partitions"

typedef enum{
    CMD_GET_STATUS,
    CMD_GET_VERSION,
    CMD_CONTROL
}BASEPCB_CMD;


class MonitorPage : public PageWidget
{
    Q_OBJECT
public:
    explicit MonitorPage(GytOptions *options, QWidget *parent = 0);

    void setBasePcbTemp1Text(const float);
    void setBasePcbTemp2Text(const float);
    void setFanSpeedText(const float);
    void setFwVerText(const QString);
    void setHwVerText(const QString);
    void setCpuDuty(const float);

    bool sockToBasdPcbIsOk;

public slots:
    void updateSysParam(void);

private slots:
    void on_updateTimer_timeout(void);
    void fanModeBtnClicked(void);
    void changeFanSpeedBtnClicked(void);

private:
    QGroupBox *cpuGroupBox;
    QGroupBox *tempGroupBox;
    QGroupBox *memGroupBox;
    QGroupBox *diskGroupBox;

    long totalCpuNum;
    long availCpuNum;
    float cpuTotalDuty;
    //float cpuDuty[];
    QLabel *cpuNameLabel;
    QLabel *cpuVendorLabel;
    QLabel *cpuCoreLabel;
    QLabel *cpuTotalDutyLabel;
    QProgressBar *cpuBar;

    float armTemp;
    float adspTemp;
    float pcbTemp;
    float fanSpeed;
    QLabel *tempLabel;
    QLabel *armTempLabel;
    QLabel *adspTempLabel;
    QLabel *pcbTempLabel;
    QLabel *fanSpeedLabel;
    QLabel *fwVerLabel;
    QLabel *hwVerLabel;

    uint memTotal;
    uint memUsed;
    uint memFree;
    QLabel *memTotalLabel;
    QLabel *memUsedLabel;
    QLabel *memFreeLabel;
    QProgressBar *memBar;

    uint diskTotal;
    uint diskUsed;
    uint diskFree;
    QLabel *diskTotalLabel;
    QLabel *diskUsedLabel;
    QLabel *diskFreeLabel;
    QProgressBar *diskBar;

    QTimer *updateTimer;


    int sockfd;
    int fanMode;

    void createSocketWithBasePcb(void);
    void sendBasePcbCmd(BASEPCB_CMD which, const QString & arg);
};

#endif // MONITORPAGE_H

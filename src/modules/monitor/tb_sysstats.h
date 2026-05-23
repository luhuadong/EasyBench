#ifndef TB_SYSSTATS_H
#define TB_SYSSTATS_H

#include <QString>

namespace TbSysStats {

struct MemInfo {
    uint totalMb = 0;
    uint usedMb = 0;
    uint freeMb = 0;
};

struct DiskInfo {
    uint totalMb = 0;
    uint usedMb = 0;
    uint freeMb = 0;
};

struct CpuInfo {
    QString modelName;
    QString hardware;
    QString displayCpuName;
    QString displayGpuName;
    long totalCores = 0;
    long onlineCores = 0;
};

bool readArmTemperature(float *celsius);
bool readMemInfo(MemInfo *out);
bool readDiskInfo(const QString &mountPoint, DiskInfo *out);
bool readCpuInfo(CpuInfo *out);
QString readRtcDateTime();

} // namespace TbSysStats

#endif /* TB_SYSSTATS_H */

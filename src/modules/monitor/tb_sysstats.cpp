#include "tb_sysstats.h"

#include <QFile>
#include <QRegExp>

extern "C" {
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <unistd.h>
}

namespace TbSysStats {

static uint parseMeminfoKbValue(const QString &line)
{
    const QString valuePart = line.section(QLatin1Char(':'), 1).trimmed();
    return valuePart.split(QLatin1Char(' '), QString::SkipEmptyParts).value(0).toUInt();
}

bool readArmTemperature(float *celsius)
{
    if (!celsius) {
        return false;
    }
    QFile file(QStringLiteral("/sys/class/thermal/thermal_zone0/temp"));
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    const QString raw = QString::fromUtf8(file.readLine()).trimmed();
    bool ok = false;
    const int milli = raw.toInt(&ok);
    if (!ok) {
        return false;
    }
    *celsius = milli / 1000.0f;
    return true;
}

bool readMemInfo(MemInfo *out)
{
    if (!out) {
        return false;
    }

    struct sysinfo si;
    if (sysinfo(&si) == 0 && si.mem_unit > 0) {
        const unsigned long long totalBytes =
            static_cast<unsigned long long>(si.totalram) * si.mem_unit;
        const unsigned long long freeBytes =
            static_cast<unsigned long long>(si.freeram) * si.mem_unit;
        out->totalMb = static_cast<uint>(totalBytes >> 20);
        out->freeMb = static_cast<uint>(freeBytes >> 20);
        out->usedMb = out->totalMb > out->freeMb ? out->totalMb - out->freeMb : 0;
    }

    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        return out->totalMb > 0;
    }

    uint totalKb = 0;
    uint freeKb = 0;
    uint availableKb = 0;
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine());
        if (line.startsWith(QStringLiteral("MemTotal:"))) {
            totalKb = parseMeminfoKbValue(line);
        } else if (line.startsWith(QStringLiteral("MemFree:"))) {
            freeKb = parseMeminfoKbValue(line);
        } else if (line.startsWith(QStringLiteral("MemAvailable:"))) {
            availableKb = parseMeminfoKbValue(line);
        }
    }

    if (totalKb > 0) {
        out->totalMb = totalKb / 1024;
        if (availableKb > 0) {
            out->freeMb = availableKb / 1024;
        } else {
            out->freeMb = freeKb / 1024;
        }
        out->usedMb = out->totalMb > out->freeMb ? out->totalMb - out->freeMb : 0;
    }
    return out->totalMb > 0;
}

bool readDiskInfo(const QString &mountPoint, DiskInfo *out)
{
    if (!out) {
        return false;
    }
    struct statfs diskInfo;
    if (statfs(mountPoint.toUtf8().constData(), &diskInfo) != 0) {
        return false;
    }
    const unsigned long long blockSize = diskInfo.f_bsize;
    const unsigned long long totalSize = blockSize * diskInfo.f_blocks;
    const unsigned long long freeSize = blockSize * diskInfo.f_bfree;
    out->totalMb = static_cast<uint>(totalSize >> 20);
    out->freeMb = static_cast<uint>(freeSize >> 20);
    out->usedMb = out->totalMb > out->freeMb ? out->totalMb - out->freeMb : 0;
    return true;
}

bool readCpuInfo(CpuInfo *out)
{
    if (!out) {
        return false;
    }
    out->totalCores = sysconf(_SC_NPROCESSORS_CONF);
    out->onlineCores = sysconf(_SC_NPROCESSORS_ONLN);

    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine());
        const int colon = line.indexOf(QLatin1Char(':'));
        if (colon < 0) {
            continue;
        }
        const QString key = line.left(colon).trimmed();
        const QString value = line.mid(colon + 1).trimmed();

        if (key.compare(QStringLiteral("model name"), Qt::CaseInsensitive) == 0
            || key.compare(QStringLiteral("Model"), Qt::CaseInsensitive) == 0) {
            if (out->modelName.isEmpty()) {
                out->modelName = value;
            }
        } else if (key.compare(QStringLiteral("Hardware"), Qt::CaseInsensitive) == 0) {
            out->hardware = value;
        } else if (key.compare(QStringLiteral("vendor_id"), Qt::CaseInsensitive) == 0) {
            if (out->hardware.isEmpty()) {
                out->hardware = value;
            }
        } else if (key.compare(QStringLiteral("Processor"), Qt::CaseInsensitive) == 0) {
            if (out->modelName.isEmpty()) {
                out->modelName = value;
            }
        }
    }

    if (out->modelName.isEmpty()) {
        out->modelName = QStringLiteral("Unknown CPU");
    }
    return true;
}

QString readRtcDateTime()
{
    QFile dateFile(QStringLiteral("/sys/class/rtc/rtc0/date"));
    QFile timeFile(QStringLiteral("/sys/class/rtc/rtc0/time"));
    if (dateFile.open(QIODevice::ReadOnly) && timeFile.open(QIODevice::ReadOnly)) {
        const QString date = QString::fromUtf8(dateFile.readAll()).trimmed();
        const QString time = QString::fromUtf8(timeFile.readAll()).trimmed();
        if (!date.isEmpty() && !time.isEmpty()) {
            return date + QLatin1Char(' ') + time;
        }
    }

    FILE *stream = popen("hwclock --show 2>/dev/null", "r");
    if (!stream) {
        stream = popen("hwclock -r 2>/dev/null", "r");
    }
    if (stream) {
        char buf[128] = {0};
        if (fgets(buf, sizeof(buf), stream)) {
            pclose(stream);
            return QString::fromUtf8(buf).trimmed();
        }
        pclose(stream);
    }
    return QString();
}

} // namespace TbSysStats

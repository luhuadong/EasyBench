#include "tb_sysstats.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegExp>
#include <QSet>

extern "C" {
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <unistd.h>
}

namespace TbSysStats {

static int indexOfKeyValueColon(const QString &line)
{
    int colon = line.indexOf(QLatin1Char(':'));
    if (colon < 0) {
        colon = line.indexOf(QChar(0xFF1A));
    }
    return colon;
}

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

/** Model of lscpu is a number; Model name is the full CPU name. */
static bool isLscpuModelNameKey(const QString &key)
{
    return key.compare(QStringLiteral("Model name"), Qt::CaseInsensitive) == 0
           || key.compare(QStringLiteral("型号名称"), Qt::CaseInsensitive) == 0;
}

static bool isLscpuMaxMhzKey(const QString &key)
{
    return key.compare(QStringLiteral("CPU max MHz"), Qt::CaseInsensitive) == 0
           || key.contains(QStringLiteral("CPU max"), Qt::CaseInsensitive)
           || key.contains(QStringLiteral("最大 MHz"), Qt::CaseInsensitive);
}

static bool looksLikeCpuMarketingName(const QString &value)
{
    if (value.length() < 8) {
        return false;
    }
    return value.contains(QLatin1Char(' ')) || value.contains(QLatin1Char('-'));
}

struct LscpuInfo {
    QString modelName;
    double maxMhz = 0.0;
};

static LscpuInfo readLscpuInfo()
{
    LscpuInfo info;
    QProcess proc;
    proc.start(QStringLiteral("lscpu"));
    if (!proc.waitForFinished(2000) || proc.exitCode() != 0) {
        return info;
    }

    const QString output = QString::fromUtf8(proc.readAllStandardOutput());
    for (const QString &line : output.split(QLatin1Char('\n'))) {
        const int colon = indexOfKeyValueColon(line);
        if (colon < 0) {
            continue;
        }
        const QString key = line.left(colon).trimmed();
        const QString value = line.mid(colon + 1).trimmed();

        if (info.modelName.isEmpty() && isLscpuModelNameKey(key)
            && looksLikeCpuMarketingName(value)) {
            info.modelName = value;
        }
        if (info.maxMhz <= 0.0 && isLscpuMaxMhzKey(key)) {
            bool ok = false;
            const double mhz = value.toDouble(&ok);
            if (ok && mhz > 0.0) {
                info.maxMhz = mhz;
            }
        }
    }
    return info;
}

static void fillDisplayCpuName(CpuInfo *out);
static void fillDisplayGpuName(CpuInfo *out);

bool readCpuInfo(CpuInfo *out)
{
    if (!out) {
        return false;
    }

    out->modelName.clear();
    out->hardware.clear();
    out->displayCpuName.clear();
    out->displayGpuName.clear();
    out->totalCores = sysconf(_SC_NPROCESSORS_CONF);
    out->onlineCores = sysconf(_SC_NPROCESSORS_ONLN);

    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            const QString line = QString::fromUtf8(file.readLine());
            const int colon = indexOfKeyValueColon(line);
            if (colon < 0) {
                continue;
            }
            const QString key = line.left(colon).trimmed();
            const QString value = line.mid(colon + 1).trimmed();

            if (key.compare(QStringLiteral("model name"), Qt::CaseInsensitive) == 0) {
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
    }

    if (out->modelName.isEmpty()) {
        out->modelName = QStringLiteral("Unknown CPU");
    }

    fillDisplayCpuName(out);
    fillDisplayGpuName(out);
    return true;
}

static QString readDeviceTreeModel()
{
    QFile file(QStringLiteral("/sys/firmware/devicetree/base/model"));
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QByteArray raw = file.readAll();
    const int nul = raw.indexOf('\0');
    if (nul >= 0) {
        raw.truncate(nul);
    }
    return QString::fromUtf8(raw).trimmed();
}

static QString shortenCpuBrand(const QString &input)
{
    QString brand = input;
    brand.replace(QStringLiteral("Intel(R)"), QStringLiteral("Intel"));
    brand.replace(QStringLiteral("Intel (R)"), QStringLiteral("Intel"));
    brand.replace(QStringLiteral("Core(TM)"), QString());
    brand.replace(QStringLiteral("Core (TM)"), QString());
    static const QRegExp atFreqRx(QStringLiteral("\\s*@\\s*[0-9.]+\\s*GHz\\s*$"), Qt::CaseInsensitive);
    brand.remove(atFreqRx);
    while (brand.contains(QStringLiteral("  "))) {
        brand.replace(QStringLiteral("  "), QStringLiteral(" "));
    }
    brand = brand.trimmed();
    return brand.isEmpty() ? input.trimmed() : brand;
}

static double readCpuMaxFrequencyGhzFromSysfs()
{
    QFile freqFile(QStringLiteral("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"));
    if (!freqFile.open(QIODevice::ReadOnly)) {
        return 0.0;
    }
    bool ok = false;
    const qint64 khz = QString::fromUtf8(freqFile.readAll()).trimmed().toLongLong(&ok);
    if (!ok || khz <= 0) {
        return 0.0;
    }
    return khz / 1000000.0;
}

static void fillDisplayCpuName(CpuInfo *out)
{
    if (!out) {
        return;
    }

    const LscpuInfo lscpu = readLscpuInfo();
    QString brand;
    bool useNeofetchExtras = false;

    if (!lscpu.modelName.isEmpty()) {
        brand = lscpu.modelName;
        useNeofetchExtras = true;
    } else if (out->modelName != QStringLiteral("Unknown CPU")) {
        brand = out->modelName;
        useNeofetchExtras = true;
    } else {
        brand = readDeviceTreeModel();
    }

    if (brand.isEmpty()) {
        out->displayCpuName.clear();
        return;
    }

    if (useNeofetchExtras) {
        brand = shortenCpuBrand(brand);
    }

    const long cores = out->onlineCores > 0 ? out->onlineCores : out->totalCores;
    if (cores > 0) {
        brand += QStringLiteral(" (%1)").arg(cores);
    }

    if (useNeofetchExtras) {
        double maxGhz = readCpuMaxFrequencyGhzFromSysfs();
        if (maxGhz <= 0.01 && lscpu.maxMhz > 0.0) {
            maxGhz = lscpu.maxMhz / 1000.0;
        }
        if (maxGhz > 0.01) {
            brand += QStringLiteral(" @ %1GHz").arg(maxGhz, 0, 'f', 3);
        }
    }

    out->displayCpuName = brand;
}

static QString parseLspciDisplayName(const QString &line)
{
    const int afterClass = line.indexOf(QStringLiteral("]: "));
    if (afterClass < 0) {
        return QString();
    }
    QString name = line.mid(afterClass + 3).trimmed();
    const int pciId = name.lastIndexOf(QStringLiteral(" ["));
    if (pciId > 0) {
        name = name.left(pciId).trimmed();
    }
    return name;
}

static bool pciIdsFromLine(const QString &line, QString *vendorId, QString *deviceId)
{
    static const QRegExp pciRx(QStringLiteral("\\[([0-9a-f]{4}):([0-9a-f]{4})\\]"), Qt::CaseInsensitive);
    if (pciRx.indexIn(line) < 0) {
        return false;
    }
    if (vendorId) {
        *vendorId = pciRx.cap(1).toLower();
    }
    if (deviceId) {
        *deviceId = pciRx.cap(2).toLower();
    }
    return true;
}

static QString vendorShortNameFromPci(const QString &vendorId, const QString &fullName)
{
    if (vendorId == QStringLiteral("8086")) {
        return QStringLiteral("Intel");
    }
    if (vendorId == QStringLiteral("10de")) {
        return QStringLiteral("NVIDIA");
    }
    if (vendorId == QStringLiteral("1002") || vendorId == QStringLiteral("1022")) {
        return QStringLiteral("AMD");
    }
    QString vendor = fullName.section(QLatin1Char(' '), 0, 0);
    if (vendor.endsWith(QLatin1Char(','))) {
        vendor.chop(1);
    }
    return vendor;
}

static bool isKnownGpuProductName(const QString &name)
{
    static const QStringList products = {
        QStringLiteral("GeForce"),  QStringLiteral("Radeon"),   QStringLiteral("Arc "),
        QStringLiteral("UHD"),      QStringLiteral("Iris"),     QStringLiteral("HD Graphics"),
        QStringLiteral("Quadro"),   QStringLiteral("Tesla"),    QStringLiteral("RTX "),
        QStringLiteral("GTX "),     QStringLiteral("RX "),      QStringLiteral("Mali"),
        QStringLiteral("Adreno"),  QStringLiteral("Tegra"),   QStringLiteral("Matrox"),
    };
    for (const QString &token : products) {
        if (name.contains(token, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

/** neofetch-like: generic PCI devices are displayed as "Intel Device 9a49". */
static QString formatGpuLineNeofetch(const QString &line)
{
    QString fullName = parseLspciDisplayName(line);
    if (fullName.isEmpty()) {
        return QString();
    }

    QString vendorId;
    QString deviceId;
    pciIdsFromLine(line, &vendorId, &deviceId);

    const bool genericName = fullName.contains(QStringLiteral("Device"), Qt::CaseInsensitive)
                             && !isKnownGpuProductName(fullName);
    if (!deviceId.isEmpty() && genericName) {
        const QString vendor = vendorShortNameFromPci(vendorId, fullName);
        return QStringLiteral("%1 Device %2").arg(vendor, deviceId);
    }

    fullName.replace(QStringLiteral("Corporation"), QString());
    fullName.replace(QStringLiteral("  "), QStringLiteral(" "));
    return fullName.trimmed();
}

static bool isGpuLspciLine(const QString &line)
{
    return line.contains(QStringLiteral("VGA compatible controller"), Qt::CaseInsensitive)
           || line.contains(QStringLiteral("3D controller"), Qt::CaseInsensitive)
           || line.contains(QStringLiteral("Display controller"), Qt::CaseInsensitive);
}

static QString readGpuNameFromDrmSysfs()
{
    QDir drmDir(QStringLiteral("/sys/class/drm"));
    if (!drmDir.exists()) {
        return QString();
    }

    const QStringList cards =
        drmDir.entryList(QStringList() << QStringLiteral("card*"), QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList names;
    QSet<QString> seen;

    for (const QString &card : cards) {
        if (card.contains(QLatin1Char('-'))) {
            continue;
        }
        const QString deviceDir = drmDir.filePath(card + QStringLiteral("/device/"));

        auto readTrimmedFile = [](const QString &path) -> QString {
            QFile f(path);
            if (!f.open(QIODevice::ReadOnly)) {
                return QString();
            }
            return QString::fromUtf8(f.readAll()).trimmed();
        };

        QString name = readTrimmedFile(deviceDir + QStringLiteral("product_name"));
        if (name.isEmpty()) {
            name = readTrimmedFile(deviceDir + QStringLiteral("label"));
        }
        if (name.isEmpty()) {
            QFile uevent(deviceDir + QStringLiteral("uevent"));
            if (uevent.open(QIODevice::ReadOnly)) {
                const QString text = QString::fromUtf8(uevent.readAll());
                for (const QString &line : text.split(QLatin1Char('\n'))) {
                    if (line.startsWith(QStringLiteral("DRIVER="))) {
                        const QString driver = line.section(QLatin1Char('='), 1).trimmed();
                        if (!driver.isEmpty()) {
                            name = driver;
                        }
                        break;
                    }
                }
            }
        }
        if (!name.isEmpty() && !seen.contains(name)) {
            seen.insert(name);
            names.append(name);
        }
    }

    return names.join(QStringLiteral(", "));
}

static QString readGpuNameFromLspci()
{
    QProcess proc;
    proc.start(QStringLiteral("lspci"), QStringList() << QStringLiteral("-nn"));
    if (!proc.waitForFinished(3000) || proc.exitCode() != 0) {
        return QString();
    }

    QStringList names;
    QSet<QString> seen;
    const QString output = QString::fromUtf8(proc.readAllStandardOutput());
    for (const QString &line : output.split(QLatin1Char('\n'))) {
        if (!isGpuLspciLine(line)) {
            continue;
        }
        const QString name = formatGpuLineNeofetch(line);
        if (!name.isEmpty() && !seen.contains(name)) {
            seen.insert(name);
            names.append(name);
        }
    }
    return names.join(QStringLiteral(", "));
}

static void fillDisplayGpuName(CpuInfo *out)
{
    if (!out) {
        return;
    }
    out->displayGpuName = readGpuNameFromLspci();
    if (out->displayGpuName.isEmpty()) {
        out->displayGpuName = readGpuNameFromDrmSysfs();
    }
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

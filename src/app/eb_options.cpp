#include "eb_options.h"
#include "eb_paths.h"
#include "modules/monitor/eb_version_info.h"

#include <cstring>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

extern "C" char *getBacklightNodeName(void);

EbOptions::EbOptions()
    : configOk(false)
{
    configPath = resolveConfigPath();
    configOk = !configPath.isEmpty();

    QSettings configRead(configPath.isEmpty() ? QStringLiteral("easybench.conf") : configPath,
                         QSettings::IniFormat);

    touchType = (TouchType)configRead.value(QStringLiteral("/LCD/Type"), 0).toUInt();
    backlightNode = configRead.value(QStringLiteral("LCD/Backlight_name")).toString();
    serialPortNode = configRead.value(QStringLiteral("DEVICE/Serial_port")).toString();
    videoInputNode = configRead.value(QStringLiteral("DEVICE/Video_input")).toString();
    seatRoot = resolveSeatRoot(configRead.value(QStringLiteral("PATH/SeatRoot")).toString());
    hasEeprom = configRead.value(QStringLiteral("DEVICE/EEPROM")).toBool();

    if (backlightNode.isEmpty()) {
        char *node = getBacklightNodeName();
        if (node) {
            backlightNode = QString::fromUtf8(node);
            free(node);
        }
    }

    if (!serialPortNode.isEmpty()) {
        strncpy(gSerialPortStr, serialPortNode.toUtf8().constData(), sizeof(gSerialPortStr) - 1);
        gSerialPortStr[sizeof(gSerialPortStr) - 1] = '\0';
    }

    loadVersionInfo(configRead);

    struct fb_var_screeninfo vinfo;
    if (0 == getScreenInfo(&vinfo)) {
        lcdWidth = vinfo.xres;
        lcdHeight = vinfo.yres;
    } else {
        lcdWidth = FIXED_WINDOWN_WIDTH;
        lcdHeight = FIXED_WINDOWN_HEIGHT;
    }

    fixedWidth = FIXED_WINDOWN_WIDTH;
    fixedHeight = FIXED_WINDOWN_HEIGHT;

    cameraViewWidth = 640;
    cameraViewHeight = 480;
}

EbOptions::~EbOptions()
{
}

QString EbOptions::resolveConfigPath()
{
    return EbPaths::resolveConfigFile();
}

QString EbOptions::resolveSeatRoot(const QString &configuredRoot)
{
    auto normalize = [](QString root) {
        if (root.isEmpty()) {
            return root;
        }
        if (!root.endsWith(QLatin1Char('/'))) {
            root.append(QLatin1Char('/'));
        }
        return root;
    };

    QString root = normalize(configuredRoot);

#if SEAT_TEST
    if (!root.isEmpty() && QFileInfo::exists(root + QStringLiteral("conf/chnlCfg.xml"))) {
        return root;
    }

    QStringList bases;
    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        bases << appDir + QStringLiteral("/");
        bases << appDir + QStringLiteral("/../");
        bases << appDir + QStringLiteral("/../../");
    }
    bases << QDir::currentPath() + QStringLiteral("/");

    static const QStringList seatMarkers = {
        QStringLiteral("legacy/seat/chnlCfg.xml"),
        QStringLiteral("conf/chnlCfg.xml"),
    };
    for (const QString &base : bases) {
        const QString normalized = normalize(base);
        for (const QString &marker : seatMarkers) {
            if (QFileInfo::exists(normalized + marker)) {
                return normalized;
            }
        }
    }
#endif

    return root;
}

int EbOptions::getScreenInfo(struct fb_var_screeninfo *vinfo)
{
    const int fbfd = open("/dev/fb0", O_RDONLY);
    if (fbfd < 0) {
        return -1;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, vinfo)) {
        close(fbfd);
        return -1;
    }
    close(fbfd);
    return 0;
}

void EbOptions::loadVersionInfo(const QSettings &mainConfig)
{
    const QString verPath = EbVersion::resolveVersionConfigPath();
    QSettings verCfg(verPath.isEmpty() ? QStringLiteral("easybench-version.conf") : verPath,
                     QSettings::IniFormat);

    auto cfgVer = [&](const char *key) -> QString {
        return verCfg.value(QString::fromLatin1(key)).toString().trimmed();
    };
    auto cfgMain = [&](const char *key) -> QString {
        return mainConfig.value(QString::fromLatin1(key)).toString().trimmed();
    };

    QString cfgProduct = cfgVer("Platform/Product");
    if (cfgProduct.isEmpty()) {
        cfgProduct = cfgMain("VERSION/Product");
    }
    if (cfgProduct.isEmpty()) {
        const QString manufacturer = cfgMain("PLATFORM/Manufacturer");
        const QString platformModel = cfgMain("PLATFORM/Model");
        if (!manufacturer.isEmpty() && !platformModel.isEmpty()) {
            cfgProduct = manufacturer + QLatin1Char(' ') + platformModel;
        } else if (!platformModel.isEmpty()) {
            cfgProduct = platformModel;
        }
    }
    product = EbVersion::pick(cfgProduct, EbVersion::probeProduct(), QStringLiteral("Unknown"));

    QString cfgDeveloper = cfgVer("Platform/Developer");
    developer = EbVersion::pick(cfgDeveloper, QString(), QStringLiteral("EasyBench"));

    QString cfgOs = cfgVer("Linux/OsVersion");
    osVersion = EbVersion::pick(cfgOs, EbVersion::probeOsVersion(), QStringLiteral("—"));

    QString cfgDistro = cfgVer("Linux/Distro");
    distro = EbVersion::pick(cfgDistro, EbVersion::probeDistro(), QStringLiteral("Linux"));

    QString cfgKernel = cfgVer("Linux/Kernel");
    kernel = EbVersion::pick(cfgKernel, EbVersion::probeKernel(), QStringLiteral("—"));

    QString cfgBoot = cfgVer("Linux/Bootloader");
    bootloader = EbVersion::pick(cfgBoot, EbVersion::probeBootloader(), QStringLiteral("—"));

    QString cfgGcc = cfgVer("Linux/Gcc");
    gcc = EbVersion::pick(cfgGcc, EbVersion::probeGcc(), QStringLiteral("—"));

    QString cfgModel = cfgVer("Platform/Model");
    if (cfgModel.isEmpty()) {
        cfgModel = cfgMain("PLATFORM/Model");
    }
    model = EbVersion::pick(cfgModel, EbVersion::probeHardwareModel(), QStringLiteral("—"));
}

QSize EbOptions::fixedSize() const
{
    return QSize(fixedWidth, fixedHeight);
}

QSize EbOptions::lcdSize() const
{
    return QSize(lcdWidth, lcdHeight);
}

QSize EbOptions::cameraViewSize() const
{
    return QSize(cameraViewWidth, cameraViewHeight);
}

TouchType EbOptions::getTouchType() const
{
    return touchType;
}

QString EbOptions::getBacklightNode() const
{
    return backlightNode;
}

QString EbOptions::getSeatRoot() const
{
    return seatRoot;
}

QString EbOptions::getSerialPort() const
{
    return serialPortNode;
}

bool EbOptions::configLoaded() const
{
    return configOk;
}

QString EbOptions::configFilePath() const
{
    return configPath;
}

QString EbOptions::getProductInfo() const
{
    return product;
}

QString EbOptions::getDeveloperInfo() const
{
    return developer;
}

QString EbOptions::getCustomOSInfo() const
{
    return osVersion;
}

QString EbOptions::getDistroInfo() const
{
    return distro;
}

QString EbOptions::getKernelInfo() const
{
    return kernel;
}

QString EbOptions::getBootloaderInfo() const
{
    return bootloader;
}

QString EbOptions::getGCCInfo() const
{
    return gcc;
}

QString EbOptions::getModelInfo() const
{
    return model;
}

QString EbOptions::getAppVersion() const
{
    return QString("%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION);
}

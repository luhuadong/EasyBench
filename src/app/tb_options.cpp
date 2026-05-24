#include "tb_options.h"
#include "tb_paths.h"
#include "modules/monitor/tb_version_info.h"

#include <cstring>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>

extern "C" char *getBacklightNodeName(void);

namespace {

QSize probePhysicalDisplaySize()
{
    struct fb_var_screeninfo vinfo;
    const int fbfd = open("/dev/fb0", O_RDONLY);
    if (fbfd >= 0) {
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == 0) {
            close(fbfd);
            if (vinfo.xres > 0 && vinfo.yres > 0) {
                return QSize(static_cast<int>(vinfo.xres), static_cast<int>(vinfo.yres));
            }
        } else {
            close(fbfd);
        }
    }

    if (QGuiApplication::instance() != nullptr) {
        if (QScreen *screen = QGuiApplication::primaryScreen()) {
            const QSize size = screen->geometry().size();
            if (size.isValid() && size.width() > 0 && size.height() > 0) {
                return size;
            }
        }
    }

    return QSize(FIXED_WINDOWN_WIDTH, FIXED_WINDOWN_HEIGHT);
}

} // namespace

TbOptions::TbOptions()
    : configOk(false)
{
    configPath = resolveConfigPath();
    configOk = !configPath.isEmpty();

    QSettings configRead(configPath.isEmpty() ? QStringLiteral("tuxibit.conf") : configPath,
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

    const QSize displaySize = probePhysicalDisplaySize();
    lcdWidth = static_cast<uint>(displaySize.width());
    lcdHeight = static_cast<uint>(displaySize.height());

    fixedWidth = FIXED_WINDOWN_WIDTH;
    fixedHeight = FIXED_WINDOWN_HEIGHT;

    cameraViewWidth = 640;
    cameraViewHeight = 480;
}

TbOptions::~TbOptions()
{
}

QString TbOptions::resolveConfigPath()
{
    return TbPaths::resolveConfigFile();
}

QString TbOptions::resolveSeatRoot(const QString &configuredRoot)
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

int TbOptions::getScreenInfo(struct fb_var_screeninfo *vinfo)
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

void TbOptions::loadVersionInfo(const QSettings &mainConfig)
{
    const QString verPath = TbVersion::resolveVersionConfigPath();
    QSettings verCfg(verPath.isEmpty() ? QStringLiteral("tuxibit-version.conf") : verPath,
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
    product = TbVersion::pick(cfgProduct, TbVersion::probeProduct(), QStringLiteral("Unknown"));

    QString cfgDeveloper = cfgVer("Platform/Developer");
    developer = TbVersion::pick(cfgDeveloper, QString(), QStringLiteral("TuxiBit"));

    QString cfgOs = cfgVer("Linux/OsVersion");
    osVersion = TbVersion::pick(cfgOs, TbVersion::probeOsVersion(), QStringLiteral("—"));

    QString cfgDistro = cfgVer("Linux/Distro");
    distro = TbVersion::pick(cfgDistro, TbVersion::probeDistro(), QStringLiteral("Linux"));

    QString cfgKernel = cfgVer("Linux/Kernel");
    kernel = TbVersion::pick(cfgKernel, TbVersion::probeKernel(), QStringLiteral("—"));

    QString cfgBoot = cfgVer("Linux/Bootloader");
    bootloader = TbVersion::pick(cfgBoot, TbVersion::probeBootloader(), QStringLiteral("—"));

    QString cfgGcc = cfgVer("Linux/Gcc");
    gcc = TbVersion::pick(cfgGcc, TbVersion::probeGcc(), QStringLiteral("—"));

    QString cfgModel = cfgVer("Platform/Model");
    if (cfgModel.isEmpty()) {
        cfgModel = cfgMain("PLATFORM/Model");
    }
    model = TbVersion::pick(cfgModel, TbVersion::probeHardwareModel(), QStringLiteral("—"));
}

QSize TbOptions::fixedSize() const
{
    return QSize(fixedWidth, fixedHeight);
}

QSize TbOptions::lcdSize() const
{
    return probePhysicalDisplaySize();
}

QSize TbOptions::cameraViewSize() const
{
    return QSize(cameraViewWidth, cameraViewHeight);
}

TouchType TbOptions::getTouchType() const
{
    return touchType;
}

QString TbOptions::getBacklightNode() const
{
    return backlightNode;
}

QString TbOptions::getSeatRoot() const
{
    return seatRoot;
}

QString TbOptions::getSerialPort() const
{
    return serialPortNode;
}

bool TbOptions::configLoaded() const
{
    return configOk;
}

QString TbOptions::configFilePath() const
{
    return configPath;
}

QString TbOptions::getProductInfo() const
{
    return product;
}

QString TbOptions::getDeveloperInfo() const
{
    return developer;
}

QString TbOptions::getCustomOSInfo() const
{
    return osVersion;
}

QString TbOptions::getDistroInfo() const
{
    return distro;
}

QString TbOptions::getKernelInfo() const
{
    return kernel;
}

QString TbOptions::getBootloaderInfo() const
{
    return bootloader;
}

QString TbOptions::getGCCInfo() const
{
    return gcc;
}

QString TbOptions::getModelInfo() const
{
    return model;
}

QString TbOptions::getAppVersion() const
{
    return QString("%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION);
}

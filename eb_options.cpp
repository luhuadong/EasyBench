#include "eb_options.h"

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

    QString verFile = QStringLiteral("/etc/gy-version");
    QSettings configRead2(verFile, QSettings::IniFormat);

    product = configRead2.value(QStringLiteral("/PLATFORM/Product"), QStringLiteral("reTerminal")).toString();
    developer = configRead2.value(QStringLiteral("/PLATFORM/Developer"), QStringLiteral("EasyBench Team")).toString();
    rootfs = configRead2.value(QStringLiteral("/LINUX/Rootfs"), QStringLiteral("1.0.0")).toString();
    gyos = configRead2.value(QStringLiteral("/LINUX/GYOS"), QStringLiteral("MyLinux")).toString()
           + QStringLiteral("_") + rootfs;
    distro = configRead2.value(QStringLiteral("/LINUX/YOCTO"), QStringLiteral("Yocto 1.8")).toString();
    kernel = configRead2.value(QStringLiteral("/LINUX/Kernel"), QStringLiteral("3.14.52")).toString();
    bootloader = configRead2.value(QStringLiteral("/LINUX/UBoot"), QStringLiteral("2015.04-g624b022")).toString();
    gcc = configRead2.value(QStringLiteral("/LINUX/GCC"), QStringLiteral("arm-poky-linux-gnueabi-gcc")).toString();
    model = configRead2.value(QStringLiteral("/PLATFORM/Model"), QStringLiteral("Raspberry Pi Compute Module 4")).toString();

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
    QStringList candidates;
    candidates << QStringLiteral("/etc/gbox/easybench.conf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/easybench.conf");
        candidates << appDir + QStringLiteral("/../easybench.conf");
    }

    candidates << QDir::currentPath() + QStringLiteral("/easybench.conf");

    for (const QString &path : candidates) {
        if (QFile::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }
    return QString();
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
    }
    bases << QDir::currentPath() + QStringLiteral("/");

    for (const QString &base : bases) {
        const QString normalized = normalize(base);
        if (QFileInfo::exists(normalized + QStringLiteral("conf/chnlCfg.xml"))) {
            return normalized;
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

QString EbOptions::invokeShell(const char *cmd) const
{
    FILE *fstream = NULL;
    char buf[128];
    memset(buf, 0, sizeof(buf));

    if (NULL == (fstream = popen(cmd, "r"))) {
        return QString();
    }

    if (NULL == fgets(buf, sizeof(buf), fstream)) {
        pclose(fstream);
        return QString();
    }

    pclose(fstream);
    return QString(buf);
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

QString EbOptions::getGYOSInfo() const
{
    return gyos;
}

QString EbOptions::getDistroInfo() const
{
    QString info = invokeShell("cat /etc/issue");

    if (info.isEmpty()) {
        return distro;
    }

    return info.replace(QString("\\n"), QString()).replace(QString("\\l"), QString());
}

QString EbOptions::getKernelInfo() const
{
    QString info = invokeShell("uname -r");

    if (info.isEmpty()) {
        return kernel;
    }

    return info;
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

QString EbOptions::getRootfsInfo() const
{
    return rootfs;
}

QString EbOptions::getAppVersion() const
{
    return QString("%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION);
}

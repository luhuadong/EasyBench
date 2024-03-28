#include "eb_options.h"

EbOptions::EbOptions()
{
    /* Read easybench.conf */
    QString cfgFile = QString("/etc/gbox/easybench.conf");
    QSettings configRead(cfgFile, QSettings::IniFormat);

    touchType = (TouchType)configRead.value("/LCD/Type", 0).toUInt();
    backlightNode  = configRead.value("LCD/Backlight_name").toString();
    serialPortNode = configRead.value("DEVICE/Serial_port").toString();
    videoInputNode = configRead.value("DEVICE/Video_input").toString();
    seatRoot  = configRead.value("PATH/SeatRoot").toString();
    hasEeprom = configRead.value("DEVICE/EEPROM").toBool();

    /* Read /etc/gy-version */
    QString verFile = QString("/etc/gy-version");
    configRead.setPath(QSettings::IniFormat, QSettings::SystemScope, verFile);
    QSettings configRead2(verFile, QSettings::IniFormat);

    product = configRead2.value("/PLATFORM/Product", QString("reTerminal")).toString();
    developer = configRead2.value("/PLATFORM/Developer", QString("EasyBench Team")).toString();
    rootfs = configRead2.value("/LINUX/Rootfs", QString("1.0.0")).toString();
    gyos = configRead2.value("/LINUX/GYOS", QString("MyLinux")).toString() + QString("_") + rootfs;
    distro = configRead2.value("/LINUX/YOCTO", QString("Yocto 1.8")).toString();
    kernel = configRead2.value("/LINUX/Kernel", QString("3.14.52")).toString();
    bootloader = configRead2.value("/LINUX/UBoot", QString("2015.04-g624b022")).toString();
    gcc = configRead2.value("/LINUX/GCC", QString("arm-poky-linux-gnueabi-gcc")).toString();
    model = configRead2.value("/PLATFORM/Model", QString("Raspberry Pi Compute Module 4")).toString();
#if 0
    if(hasEeprom) {
        QFile eepromfile("/mnt/w25q80/version");
        if(!eepromfile.open(QFile::ReadOnly | QFile::Text)) {
            baseBoard.prepend("unknown");
        }
        baseBoard.prepend(eepromfile.readLine(32));
        baseBoard.remove('\n');
        eepromfile.close();
    } else {
        baseBoard = configRead.value("/PLATFORM/BaseBoard", QString("C019 v1.3")).toString();
    }
#endif


    struct fb_var_screeninfo vinfo;
    if(0 == getScreenInfo(&vinfo)) {
        lcdWidth = vinfo.xres;
        lcdHeight = vinfo.yres;
    }else {
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

int EbOptions::getScreenInfo(struct fb_var_screeninfo *vinfo)
{
    int fbfd = 0;
    fbfd = open("/dev/fb0", O_RDONLY);
    if (!fbfd) {
        //printf("Error: cannot open framebuffer device.\n");
        return -1;
    }
    //printf("The framebuffer device was opened successfully.\n");

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, vinfo)) {
        //printf("Error reading variable information.\n");
        return -1;
    }
    close(fbfd);
    return 0;
}

QString EbOptions::invokeShell(const char *cmd)
{
    FILE *fstream = NULL;
    char buf[128];
    memset(buf, 0, sizeof(buf));

    if(NULL == (fstream = popen(cmd, "r"))) {
        return NULL;
    }

    if(NULL == fgets(buf, sizeof(buf), fstream)) {
        pclose(fstream);
        return NULL;
    }

    pclose(fstream);
    return QString(buf);
}

QSize EbOptions::fixedSize()
{
    return QSize(fixedWidth, fixedHeight);
}

QSize EbOptions::lcdSize()
{
    return QSize(lcdWidth, lcdHeight);
}

QSize EbOptions::cameraViewSize()
{
    return QSize(cameraViewWidth, cameraViewHeight);
}

TouchType EbOptions::getTouchType()
{
    return touchType;
}

QString EbOptions::getBacklightNode()
{
    return backlightNode;
}

QString EbOptions::getSeatRoot()
{
    return seatRoot;
}

QString EbOptions::getProductInfo()
{
    return product;
}

QString EbOptions::getDeveloperInfo()
{
    return developer;
}

QString EbOptions::getGYOSInfo()
{
    return gyos;
}

QString EbOptions::getDistroInfo()
{
    QString info = invokeShell("cat /etc/issue");

    if(NULL == info) {
        return distro;
    }
    else {
        info = info.replace(QString("\\n"), QString("")).replace(QString("\\l"), QString(""));
        return info;
    }
}

QString EbOptions::getKernelInfo()
{
    QString info = invokeShell("uname -r");

    if(NULL == info) {
        return kernel;
    }
    else {
        return info;
    }
}

QString EbOptions::getBootloaderInfo()
{
    return bootloader;
}

QString EbOptions::getGCCInfo()
{
    return gcc;
}

QString EbOptions::getModelInfo()
{
    return model;
}

QString EbOptions::getRootfsInfo()
{
    return rootfs;
}

QString EbOptions::getAppVersion()
{
    return QString("%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION);
}

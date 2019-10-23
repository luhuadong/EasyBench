#include "gyt_options.h"

GytOptions::GytOptions()
{
    /* Read gyt_box.conf */
    QString cfgFile = QString(QDir::currentPath() +"/gyt_box.conf");
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

    product = configRead.value("/PLATFORM/Product", QString("GY33ASEAT")).toString();
    rootfs = configRead.value("/LINUX/Rootfs", QString("1.0.0")).toString();
    gyos = configRead.value("/LINUX/GYOS", QString("GYTLinux_GW-SV")).toString() + QString("_") + rootfs;
    yocto = configRead.value("/LINUX/YOCTO", QString("Freescale i.MX Release Distro Yocto 1.8")).toString();
    kernel = configRead.value("/LINUX/Kernel", QString("3.14.52")).toString();
    uboot = configRead.value("/LINUX/UBoot", QString("2015.04-g624b022")).toString();
    gcc = configRead.value("/LINUX/GCC", QString("arm-poky-linux-gnueabi-gcc")).toString();
    model = configRead.value("/PLATFORM/Model", QString("Advantech ROM-5420-B1")).toString();

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
}

int GytOptions::getScreenInfo(struct fb_var_screeninfo *vinfo)
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

QSize GytOptions::fixedSize()
{
    return QSize(fixedWidth, fixedHeight);
}

QSize GytOptions::lcdSize()
{
    return QSize(lcdWidth, lcdHeight);
}

TouchType GytOptions::getTouchType()
{
    return touchType;
}

QString GytOptions::getSeatRoot()
{
    return seatRoot;
}

#ifndef GYT_OPTIONS_H
#define GYT_OPTIONS_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QSize>
#include "gyt_common.h"

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
//#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
}
#endif

class GytOptions
{
public:
    GytOptions();
    ~GytOptions();

    QSize fixedSize();
    QSize lcdSize();
    QSize cameraViewSize();

    TouchType getTouchType();
    QString getBacklightNode();

    QString getSeatRoot();

    QString getProductInfo();
    QString getDeveloperInfo();
    QString getGYOSInfo();
    QString getDistroInfo();
    QString getKernelInfo();
    QString getBootloaderInfo();
    QString getGCCInfo();
    QString getModelInfo();
    QString getRootfsInfo();

    QString getAppVersion();

private:
    int getScreenInfo(struct fb_var_screeninfo *vinfo);
    QString invokeShell(const char * cmd);

private:
    QString product; /* eg. GY71L */
    QString developer; /* GYT */
    QString vendor; /* eg. Advantech */
    QString model; /* ROM-5420-B1 */
    QString processor;
    QString OS;   /* Linux */
    QString baseBoard; /* C019 v1.3 */

    QString bootloader;
    QString gcc;
    QString binutils;
    QString kernel;
    QString issue;
    QString distro;
    QString qt;
    QString rootfs;
    QString gyos;
    QString data;

    uint lcdWidth;
    uint lcdHeight;
    uint fixedWidth;
    uint fixedHeight;
    uint cameraViewWidth;
    uint cameraViewHeight;

    TouchType touchType; /* should be enum */

    QString backlightNode;
    QString serialPortNode;
    QString videoInputNode;
    bool hasEeprom;

    QString seatRoot;  /* path */
};

#endif // GYT_OPTIONS_H

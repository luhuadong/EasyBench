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

    QSize fixedSize();
    QSize lcdSize();

    TouchType getTouchType();

    QString getSeatRoot();

private:
    int getScreenInfo(struct fb_var_screeninfo *vinfo);

private:
    QString product; /* eg. GY71L */
    QString vendor; /* eg. Advantech */
    QString model; /* ROM-5420-B1 */
    QString processor;
    QString OS;   /* Linux */
    QString baseBoard; /* C019 v1.3 */

    QString uboot;
    QString gcc;
    QString binutils;
    QString kernel;
    QString issue;
    QString yocto;
    QString qt;
    QString rootfs;
    QString gyos;
    QString data;

    uint lcdWidth;
    uint lcdHeight;
    uint fixedWidth;
    uint fixedHeight;

    TouchType touchType; /* should be enum */

    QString backlightNode;
    QString serialPortNode;
    QString videoInputNode;
    bool hasEeprom;

    QString seatRoot;  /* path */
};

#endif // GYT_OPTIONS_H

#ifndef EB_OPTIONS_H
#define EB_OPTIONS_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QSize>
#include "eb_common.h"

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

class EbOptions
{
public:
    EbOptions();
    ~EbOptions();

    QSize fixedSize() const;
    QSize lcdSize() const;
    QSize cameraViewSize() const;

    TouchType getTouchType() const;
    QString getBacklightNode() const;

    QString getSeatRoot() const;

    QString getProductInfo() const;
    QString getDeveloperInfo() const;
    QString getGYOSInfo() const;
    QString getDistroInfo() const;
    QString getKernelInfo() const;
    QString getBootloaderInfo() const;
    QString getGCCInfo() const;
    QString getModelInfo() const;
    QString getRootfsInfo() const;

    QString getAppVersion() const;

    QString getSerialPort() const;
    bool configLoaded() const;
    QString configFilePath() const;

    static QString resolveConfigPath();
    static QString resolveSeatRoot(const QString &configuredRoot);

private:
    int getScreenInfo(struct fb_var_screeninfo *vinfo);
    QString invokeShell(const char *cmd) const;

private:
    QString product;
    QString developer;
    QString vendor;
    QString model;
    QString processor;
    QString OS;
    QString baseBoard;

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
    QString configPath;
    bool configOk;
};

#endif // EB_OPTIONS_H

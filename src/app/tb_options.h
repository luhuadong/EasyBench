#ifndef TB_OPTIONS_H
#define TB_OPTIONS_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QSize>
#include "tb_common.h"

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

class TbOptions
{
public:
    TbOptions();
    ~TbOptions();

    QSize fixedSize() const;
    QSize lcdSize() const;
    QSize cameraViewSize() const;

    TouchType getTouchType() const;
    QString getBacklightNode() const;

    QString getSeatRoot() const;

    QString getProductInfo() const;
    QString getDeveloperInfo() const;
    QString getCustomOSInfo() const;
    QString getDistroInfo() const;
    QString getKernelInfo() const;
    QString getBootloaderInfo() const;
    QString getGCCInfo() const;
    QString getModelInfo() const;

    QString getAppVersion() const;

    QString getSerialPort() const;
    bool configLoaded() const;
    QString configFilePath() const;

    static QString resolveConfigPath();
    static QString resolveSeatRoot(const QString &configuredRoot);

private:
    int getScreenInfo(struct fb_var_screeninfo *vinfo);
    void loadVersionInfo(const QSettings &mainConfig);

private:
    QString product;
    QString developer;
    QString model;
    QString osVersion;
    QString distro;
    QString kernel;
    QString bootloader;
    QString gcc;

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

#endif // TB_OPTIONS_H

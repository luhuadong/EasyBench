#ifndef EB_VERSION_INFO_H
#define EB_VERSION_INFO_H

#include <QString>

namespace EbVersion {

/** 配置文件 > 系统探测 > 默认值 */
QString pick(const QString &configValue,
             const QString &systemValue,
             const QString &defaultValue);

QString resolveVersionConfigPath();

QString readOsRelease(const QString &key);
QString readSysFile(const QString &path);
QString readDmiField(const QString &fieldName);

QString probeProduct();
QString probeOsVersion();
QString probeDistro();
QString probeKernel();
QString probeGcc();
QString probeBootloader();
QString probeHardwareModel();

} // namespace EbVersion

#endif // EB_VERSION_INFO_H

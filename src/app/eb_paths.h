#ifndef EB_PATHS_H
#define EB_PATHS_H

#include <QString>

namespace EbPaths {

QString resolveConfigFile();
QString resolveDeployFile(const QString &relativePath);

QString eepromArmTool();
QString i210OriginOtp();
QString chineseFontFile();

} // namespace EbPaths

#endif // EB_PATHS_H

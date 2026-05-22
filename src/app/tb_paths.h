#ifndef TB_PATHS_H
#define TB_PATHS_H

#include <QString>

namespace TbPaths {

QString resolveConfigFile();
QString resolveDeployFile(const QString &relativePath);

QString eepromArmTool();
QString i210OriginOtp();
QString chineseFontFile();

} // namespace TbPaths

#endif // TB_PATHS_H

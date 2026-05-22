#include "tb_paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace TbPaths {

static QString firstExisting(const QStringList &candidates)
{
    for (const QString &path : candidates) {
        if (QFileInfo::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }
    return QString();
}

QString resolveConfigFile()
{
    QStringList candidates;
    candidates << QStringLiteral("/etc/tuxibit/tuxibit.conf");
    candidates << QStringLiteral("/etc/gbox/tuxibit.conf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/tuxibit/tuxibit.conf");
        candidates << appDir + QStringLiteral("/../config/tuxibit.conf");
        candidates << appDir + QStringLiteral("/../../config/tuxibit.conf");
#ifdef TB_PROJECT_ROOT
        candidates << QStringLiteral(TB_PROJECT_ROOT) + QStringLiteral("/config/tuxibit.conf");
#endif
    }

    candidates << QDir::currentPath() + QStringLiteral("/config/tuxibit.conf");

    return firstExisting(candidates);
}

QString resolveDeployFile(const QString &relativePath)
{
    const QString rel = relativePath.startsWith(QLatin1Char('/'))
        ? relativePath.mid(1)
        : relativePath;

    QStringList candidates;
    candidates << QStringLiteral("/usr/share/tuxibit/deploy/") + rel;
    candidates << QStringLiteral("/usr/local/share/tuxibit/deploy/") + rel;

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/tuxibit/deploy/") + rel;
        candidates << appDir + QStringLiteral("/../deploy/") + rel;
        candidates << appDir + QStringLiteral("/../../deploy/") + rel;
#ifdef TB_PROJECT_ROOT
        candidates << QStringLiteral(TB_PROJECT_ROOT) + QStringLiteral("/deploy/") + rel;
#endif
    }

    candidates << QDir::currentPath() + QStringLiteral("/deploy/") + rel;

    return firstExisting(candidates);
}

QString eepromArmTool()
{
    const QString path = resolveDeployFile(QStringLiteral("bin/eepromARMtool"));
    if (!path.isEmpty()) {
        return path;
    }
    return QStringLiteral("eepromARMtool");
}

QString i210OriginOtp()
{
    const QString path = resolveDeployFile(QStringLiteral("data/I210NIC-origin.otp"));
    if (!path.isEmpty()) {
        return path;
    }
    return QStringLiteral("deploy/data/I210NIC-origin.otp");
}

QString chineseFontFile()
{
    QStringList candidates;
    candidates << QStringLiteral("/usr/share/tuxibit/fonts/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/local/share/tuxibit/fonts/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/share/fonts/tuxibit/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/share/fonts/ttf/LiHeiPro.ttf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/tuxibit/fonts/LiHeiPro.ttf");
        candidates << appDir + QStringLiteral("/../../resource/fonts/LiHeiPro.ttf");
#ifdef TB_PROJECT_ROOT
        candidates << QStringLiteral(TB_PROJECT_ROOT) + QStringLiteral("/resource/fonts/LiHeiPro.ttf");
#endif
    }

    candidates << QDir::currentPath() + QStringLiteral("/resource/fonts/LiHeiPro.ttf");

    return firstExisting(candidates);
}

} // namespace TbPaths

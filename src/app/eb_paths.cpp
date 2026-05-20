#include "eb_paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace EbPaths {

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
    candidates << QStringLiteral("/etc/easybench/easybench.conf");
    candidates << QStringLiteral("/etc/gbox/easybench.conf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/easybench/easybench.conf");
        candidates << appDir + QStringLiteral("/../config/easybench.conf");
        candidates << appDir + QStringLiteral("/../../config/easybench.conf");
#ifdef EB_PROJECT_ROOT
        candidates << QStringLiteral(EB_PROJECT_ROOT) + QStringLiteral("/config/easybench.conf");
#endif
    }

    candidates << QDir::currentPath() + QStringLiteral("/config/easybench.conf");

    return firstExisting(candidates);
}

QString resolveDeployFile(const QString &relativePath)
{
    const QString rel = relativePath.startsWith(QLatin1Char('/'))
        ? relativePath.mid(1)
        : relativePath;

    QStringList candidates;
    candidates << QStringLiteral("/usr/share/easybench/deploy/") + rel;
    candidates << QStringLiteral("/usr/local/share/easybench/deploy/") + rel;

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/easybench/deploy/") + rel;
        candidates << appDir + QStringLiteral("/../deploy/") + rel;
        candidates << appDir + QStringLiteral("/../../deploy/") + rel;
#ifdef EB_PROJECT_ROOT
        candidates << QStringLiteral(EB_PROJECT_ROOT) + QStringLiteral("/deploy/") + rel;
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
    candidates << QStringLiteral("/usr/share/easybench/fonts/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/local/share/easybench/fonts/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/share/fonts/easybench/LiHeiPro.ttf");
    candidates << QStringLiteral("/usr/share/fonts/ttf/LiHeiPro.ttf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../share/easybench/fonts/LiHeiPro.ttf");
        candidates << appDir + QStringLiteral("/../../resource/fonts/LiHeiPro.ttf");
#ifdef EB_PROJECT_ROOT
        candidates << QStringLiteral(EB_PROJECT_ROOT) + QStringLiteral("/resource/fonts/LiHeiPro.ttf");
#endif
    }

    candidates << QDir::currentPath() + QStringLiteral("/resource/fonts/LiHeiPro.ttf");

    return firstExisting(candidates);
}

} // namespace EbPaths

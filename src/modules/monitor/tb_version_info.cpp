#include "tb_version_info.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>

namespace TbVersion {

QString pick(const QString &configValue,
             const QString &systemValue,
             const QString &defaultValue)
{
    const QString cfg = configValue.trimmed();
    if (!cfg.isEmpty()) {
        return cfg;
    }
    const QString sys = systemValue.trimmed();
    if (!sys.isEmpty()) {
        return sys;
    }
    return defaultValue;
}

QString resolveVersionConfigPath()
{
    QStringList candidates;
    candidates << QStringLiteral("/etc/tuxibit/version.conf");
    candidates << QStringLiteral("/etc/tuxibit-version.conf");

    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/tuxibit-version.conf");
        candidates << appDir + QStringLiteral("/../config/tuxibit-version.conf");
    }

    candidates << QDir::currentPath() + QStringLiteral("/tuxibit-version.conf");
    candidates << QDir::currentPath() + QStringLiteral("/config/tuxibit-version.conf");

    for (const QString &path : candidates) {
        if (QFile::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }
    return QString();
}

static QString unquoteOsReleaseValue(const QString &raw)
{
    QString value = raw.trimmed();
    if (value.size() >= 2
        && ((value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"')))
            || (value.startsWith(QLatin1Char('\'')) && value.endsWith(QLatin1Char('\''))))) {
        return value.mid(1, value.size() - 2);
    }
    return value;
}

QString readOsRelease(const QString &key)
{
    QFile file(QStringLiteral("/etc/os-release"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    const QByteArray keyPrefix = (key + QLatin1Char('=')).toUtf8();
    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        if (line.startsWith(keyPrefix)) {
            return unquoteOsReleaseValue(QString::fromUtf8(line.mid(keyPrefix.size())));
        }
    }
    return QString();
}

QString readSysFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QByteArray data = file.readAll();
    const int nul = data.indexOf('\0');
    if (nul >= 0) {
        data.truncate(nul);
    }
    return QString::fromUtf8(data).trimmed();
}

QString readDmiField(const QString &fieldName)
{
    const QString path = QStringLiteral("/sys/class/dmi/id/") + fieldName;
    return readSysFile(path);
}

static QString runCommand(const QString &program, const QStringList &args)
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(program, args);
    if (!proc.waitForFinished(3000) || proc.exitCode() != 0) {
        return QString();
    }
    return QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
}

QString probeProduct()
{
    QString product = readDmiField(QStringLiteral("product_name"));
    if (!product.isEmpty() && product != QStringLiteral("To be filled by O.E.M.")) {
        const QString vendor = readDmiField(QStringLiteral("sys_vendor"));
        if (!vendor.isEmpty()
            && vendor != QStringLiteral("To be filled by O.E.M.")
            && !product.contains(vendor, Qt::CaseInsensitive)) {
            return vendor + QLatin1Char(' ') + product;
        }
        return product;
    }

    product = readOsRelease(QStringLiteral("PRETTY_NAME"));
    if (!product.isEmpty()) {
        return product;
    }

    const QString host = runCommand(QStringLiteral("hostname"), QStringList());
    if (!host.isEmpty()) {
        return host;
    }
    return QString();
}

QString probeOsVersion()
{
    QString version = readOsRelease(QStringLiteral("VERSION_ID"));
    if (version.isEmpty()) {
        version = readOsRelease(QStringLiteral("VERSION"));
    }
    if (!version.isEmpty()) {
        const QString buildId = readOsRelease(QStringLiteral("BUILD_ID"));
        if (!buildId.isEmpty()) {
            return version + QLatin1Char(' ') + QStringLiteral("(build ") + buildId + QLatin1Char(')');
        }
        return version;
    }
    return QString();
}

QString probeDistro()
{
    QString pretty = readOsRelease(QStringLiteral("PRETTY_NAME"));
    if (!pretty.isEmpty()) {
        return pretty;
    }

    const QString name = readOsRelease(QStringLiteral("NAME"));
    const QString versionId = readOsRelease(QStringLiteral("VERSION_ID"));
    if (!name.isEmpty() && !versionId.isEmpty()) {
        return name + QLatin1Char(' ') + versionId;
    }
    if (!name.isEmpty()) {
        return name;
    }

    QFile issue(QStringLiteral("/etc/issue"));
    if (issue.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString line = QString::fromUtf8(issue.readLine()).trimmed();
        line.replace(QStringLiteral("\\n"), QString());
        line.replace(QStringLiteral("\\l"), QString());
        line.replace(QStringLiteral("\\r"), QString());
        if (!line.isEmpty()) {
            return line;
        }
    }
    return QString();
}

QString probeKernel()
{
    return runCommand(QStringLiteral("uname"), QStringList(QStringLiteral("-r")));
}

QString probeGcc()
{
    QString full = runCommand(QStringLiteral("gcc"), QStringList(QStringLiteral("-dumpfullversion")));
    if (!full.isEmpty()) {
        return QStringLiteral("gcc ") + full;
    }

    full = runCommand(QStringLiteral("gcc"), QStringList(QStringLiteral("-dumpversion")));
    if (!full.isEmpty()) {
        return QStringLiteral("gcc ") + full;
    }

    const QString versionLine = runCommand(QStringLiteral("gcc"), QStringList(QStringLiteral("--version")));
    if (!versionLine.isEmpty()) {
        return versionLine.split(QLatin1Char('\n')).value(0).trimmed();
    }
    return QString();
}

QString probeBootloader()
{
    QString ver = runCommand(QStringLiteral("fw_printenv"), QStringList(QStringLiteral("ver")));
    if (!ver.isEmpty()) {
        const int eq = ver.indexOf(QLatin1Char('='));
        if (eq >= 0) {
            ver = ver.mid(eq + 1).trimmed();
        }
        return ver;
    }

    ver = runCommand(QStringLiteral("fw_printenv"), QStringList(QStringLiteral("version")));
    if (!ver.isEmpty()) {
        const int eq = ver.indexOf(QLatin1Char('='));
        if (eq >= 0) {
            ver = ver.mid(eq + 1).trimmed();
        }
        return ver;
    }

    const QString cmdline = readSysFile(QStringLiteral("/proc/cmdline"));
    if (cmdline.contains(QStringLiteral("U-Boot"), Qt::CaseInsensitive)) {
        static const QRegularExpression ubootRe(
            QStringLiteral("U-Boot[^\\s]*"),
            QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch match = ubootRe.match(cmdline);
        if (match.hasMatch()) {
            return match.captured(0);
        }
    }
    return QString();
}

QString probeHardwareModel()
{
    QStringList dtPaths;
    dtPaths << QStringLiteral("/proc/device-tree/model");
    dtPaths << QStringLiteral("/sys/firmware/devicetree/base/model");

    for (const QString &path : dtPaths) {
        const QString model = readSysFile(path);
        if (!model.isEmpty()) {
            return model;
        }
    }

    QString board = readDmiField(QStringLiteral("board_name"));
    if (!board.isEmpty() && board != QStringLiteral("To be filled by O.E.M.")) {
        return board;
    }

    board = readDmiField(QStringLiteral("product_name"));
    if (!board.isEmpty() && board != QStringLiteral("To be filled by O.E.M.")) {
        return board;
    }
    return QString();
}

} // namespace TbVersion

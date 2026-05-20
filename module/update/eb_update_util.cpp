#include "eb_update_util.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <unistd.h>

namespace EbUpdate {

namespace {

QString readManifestFromArchive(const QString &packagePath)
{
    const CommandResult result =
        runCommand(QStringLiteral("tar"),
                   QStringList() << QStringLiteral("-xOf") << packagePath << QLatin1String(kManifestPath),
                   8000);
    if (result.exitCode != 0) {
        return QString();
    }
    return result.standardOutput.trimmed();
}

PackageInfo parseManifestJson(const QString &jsonText, PackageInfo info)
{
    if (jsonText.isEmpty()) {
        return info;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        info.warningMessage = QStringLiteral("清单 JSON 解析失败: %1").arg(parseError.errorString());
        return info;
    }

    const QJsonObject root = doc.object();
    info.hasManifest = true;
    info.manifestFormat = root.value(QStringLiteral("format")).toInt(0);
    info.product = root.value(QStringLiteral("product")).toString().trimmed();
    info.version = root.value(QStringLiteral("version")).toString().trimmed();
    info.description = root.value(QStringLiteral("description")).toString().trimmed();
    info.rebootAfter = root.value(QStringLiteral("reboot")).toBool(false);

    if (info.manifestFormat != 0 && info.manifestFormat != 1) {
        info.warningMessage = QStringLiteral("未知清单 format=%1，将仅作提示")
                                  .arg(info.manifestFormat);
    }
    return info;
}

} // namespace

bool hasRootPrivilege()
{
    return geteuid() == 0;
}

CommandResult runCommand(const QString &program, const QStringList &args, int timeoutMs)
{
    CommandResult result;
    QProcess process;
    process.start(program, args);
    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        process.waitForFinished(3000);
        result.timedOut = true;
        return result;
    }
    result.exitCode = process.exitCode();
    result.standardOutput = QString::fromUtf8(process.readAllStandardOutput());
    result.standardError = QString::fromUtf8(process.readAllStandardError());
    return result;
}

QString runShell(const QString &shellCommand, int timeoutMs)
{
    const CommandResult result = runCommand(QStringLiteral("/bin/sh"),
                                            QStringList() << QStringLiteral("-c") << shellCommand,
                                            timeoutMs);
    QString combined = result.standardOutput;
    if (!result.standardError.isEmpty()) {
        if (!combined.isEmpty()) {
            combined += QLatin1Char('\n');
        }
        combined += result.standardError;
    }
    return combined;
}

QString findRemovablePackageHint()
{
    QDir mediaDir(QStringLiteral("/run/media"));
    if (!mediaDir.exists()) {
        return QString();
    }

    const QStringList mounts = mediaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &mount : mounts) {
        QDir mountDir(mediaDir.absoluteFilePath(mount));
        const QStringList archives =
            mountDir.entryList(QStringList() << QStringLiteral("*.tar.gz"), QDir::Files);
        if (!archives.isEmpty()) {
            return mountDir.absoluteFilePath(archives.first());
        }
    }
    return QString();
}

bool productMatches(const PackageInfo &info, const QString &expectedProduct)
{
    if (info.product.isEmpty() || expectedProduct.isEmpty()) {
        return true;
    }
    return info.product.compare(expectedProduct, Qt::CaseInsensitive) == 0;
}

PackageInfo inspectPackage(const QString &path, const QString &expectedProduct)
{
    PackageInfo info;
    info.path = path;

    const QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        info.errorMessage = QStringLiteral("文件不存在或不是普通文件");
        return info;
    }
    info.fileSizeBytes = fileInfo.size();

    const CommandResult listResult =
        runCommand(QStringLiteral("tar"),
                   QStringList() << QStringLiteral("-tzf") << path,
                   120000);
    if (listResult.timedOut) {
        info.errorMessage = QStringLiteral("校验压缩包超时");
        return info;
    }
    if (listResult.exitCode != 0) {
        info.errorMessage = listResult.standardError.isEmpty()
                                ? QStringLiteral("无法读取压缩包内容")
                                : listResult.standardError.trimmed();
        return info;
    }
    info.archiveValid = true;

    const QString manifestJson = readManifestFromArchive(path);
    info = parseManifestJson(manifestJson, info);
    info.productMatch = productMatches(info, expectedProduct);
    if (!info.productMatch) {
        info.errorMessage =
            QStringLiteral("清单产品 \"%1\" 与当前设备 \"%2\" 不匹配")
                .arg(info.product, expectedProduct);
    }

    return info;
}

bool applyPackageSync(const QString &path, QString *logOut)
{
    if (!hasRootPrivilege()) {
        if (logOut) {
            *logOut = QStringLiteral("需要 root 权限");
        }
        return false;
    }

    QString escapedPath = path;
    escapedPath.replace(QLatin1String("'"), QLatin1String("'\\''"));
    QString log;
    log += runShell(QStringLiteral("tar zxf '%1' -C /").arg(escapedPath));
    log += runShell(QStringLiteral("sync"));
    if (logOut) {
        *logOut = log.trimmed();
    }
    return true;
}

UpdateRunner::UpdateRunner(QObject *parent)
    : QObject(parent)
    , m_tarProcess(new QProcess(this))
    , m_syncProcess(new QProcess(this))
{
    connect(m_tarProcess, &QProcess::readyReadStandardOutput, this, &UpdateRunner::onTarReadyRead);
    connect(m_tarProcess, &QProcess::readyReadStandardError, this, &UpdateRunner::onTarReadyRead);
    connect(m_tarProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &UpdateRunner::onTarFinished);

    connect(m_syncProcess, &QProcess::readyReadStandardOutput, this, &UpdateRunner::onTarReadyRead);
    connect(m_syncProcess, &QProcess::readyReadStandardError, this, &UpdateRunner::onTarReadyRead);
    connect(m_syncProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &UpdateRunner::onSyncFinished);
}

void UpdateRunner::start(const QString &packagePath)
{
    stop();
    m_packagePath = packagePath;
    emit lineOutput(QStringLiteral("--- tar zxf ---"));
    m_tarProcess->start(QStringLiteral("tar"),
                        QStringList() << QStringLiteral("zxf") << packagePath << QStringLiteral("-C")
                                      << QStringLiteral("/"));
}

void UpdateRunner::stop()
{
    if (m_tarProcess->state() != QProcess::NotRunning) {
        m_tarProcess->terminate();
        m_tarProcess->waitForFinished(2000);
        if (m_tarProcess->state() != QProcess::NotRunning) {
            m_tarProcess->kill();
            m_tarProcess->waitForFinished(1000);
        }
    }
    if (m_syncProcess->state() != QProcess::NotRunning) {
        m_syncProcess->terminate();
        m_syncProcess->waitForFinished(2000);
        if (m_syncProcess->state() != QProcess::NotRunning) {
            m_syncProcess->kill();
            m_syncProcess->waitForFinished(1000);
        }
    }
}

void UpdateRunner::onTarReadyRead()
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) {
        return;
    }
    const QByteArray out = process->readAllStandardOutput();
    const QByteArray err = process->readAllStandardError();
    const QString text = QString::fromUtf8(out + err);
    for (const QString &line : text.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        emit lineOutput(line);
    }
}

void UpdateRunner::onTarFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    if (exitCode != 0) {
        emit lineOutput(QStringLiteral("tar 失败，退出码 %1").arg(exitCode));
        emit finished(false, exitCode);
        return;
    }
    runSync();
}

void UpdateRunner::runSync()
{
    emit lineOutput(QStringLiteral("--- sync ---"));
    m_syncProcess->start(QStringLiteral("sync"), QStringList());
}

void UpdateRunner::onSyncFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    const bool ok = exitCode == 0;
    if (!ok) {
        emit lineOutput(QStringLiteral("sync 失败，退出码 %1").arg(exitCode));
    }
    emit finished(ok, exitCode);
}

} // namespace EbUpdate

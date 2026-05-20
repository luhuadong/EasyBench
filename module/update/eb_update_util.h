#ifndef EB_UPDATE_UTIL_H
#define EB_UPDATE_UTIL_H

#include <QObject>
#include <QProcess>
#include <QString>

namespace EbUpdate {

static const char kManifestPath[] = "easybench-update.json";

struct CommandResult {
    int exitCode = -1;
    QString standardOutput;
    QString standardError;
    bool timedOut = false;
};

struct PackageInfo {
    QString path;
    qint64 fileSizeBytes = 0;
    bool archiveValid = false;
    bool hasManifest = false;
    int manifestFormat = 0;
    QString product;
    QString version;
    QString description;
    bool rebootAfter = false;
    bool productMatch = true;
    QString errorMessage;
    QString warningMessage;
};

bool hasRootPrivilege();
CommandResult runCommand(const QString &program, const QStringList &args, int timeoutMs = 60000);
QString runShell(const QString &shellCommand, int timeoutMs = 60000);

QString findRemovablePackageHint();
PackageInfo inspectPackage(const QString &path, const QString &expectedProduct = QString());
bool productMatches(const PackageInfo &info, const QString &expectedProduct);

bool applyPackageSync(const QString &path, QString *logOut = nullptr);

class UpdateRunner : public QObject
{
    Q_OBJECT
public:
    explicit UpdateRunner(QObject *parent = nullptr);
    void start(const QString &packagePath);
    void stop();

signals:
    void lineOutput(const QString &line);
    void finished(bool success, int exitCode);

private slots:
    void onTarReadyRead();
    void onTarFinished(int exitCode, QProcess::ExitStatus status);
    void onSyncFinished(int exitCode, QProcess::ExitStatus status);

private:
    void runSync();

    QProcess *m_tarProcess = nullptr;
    QProcess *m_syncProcess = nullptr;
    QString m_packagePath;
};

} // namespace EbUpdate

#endif /* EB_UPDATE_UTIL_H */

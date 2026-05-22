#ifndef TB_NET_UTIL_H
#define TB_NET_UTIL_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace TbNet {

enum class NetworkBackend {
    Unknown = 0,
    IpRoute2,
    NetworkManager,
    Netplan,
    Ifupdown
};

struct CommandResult {
    int exitCode = -1;
    QString standardOutput;
    QString standardError;
    bool timedOut = false;
};

struct InterfaceInfo {
    QString name;
    QString mac;
    QString ipv4;
    int prefixLength = 0;
    QString netmask;
    QString gateway;
    QStringList dnsServers;
    bool linkUp = false;
    bool carrier = false;
    QString operState;
    QString displayText;
};

class PingRunner : public QObject
{
    Q_OBJECT
public:
    explicit PingRunner(QObject *parent = nullptr);
    void start(const QString &host, int count);
    void stop();

signals:
    void lineOutput(const QString &line);
    void finished(int exitCode);

private slots:
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus status);

private:
    QProcess *m_process = nullptr;
};

QString backendName(NetworkBackend backend);

NetworkBackend detectNetworkBackend();
bool hasNetAdminPrivilege();

QList<InterfaceInfo> listInterfaces();
InterfaceInfo readInterfaceConfig(const QString &ifaceName);

bool isValidMac(const QString &mac);
QString normalizeMac(const QString &mac);

CommandResult runCommand(const QString &program, const QStringList &args, int timeoutMs = 15000);
QString runShell(const QString &shellCommand, int timeoutMs = 15000);

bool applyRuntimeMac(const QString &iface, const QString &mac, QString *errorOut = nullptr);
bool applyRuntimeIpv4Static(const QString &iface,
                            const QString &ip,
                            int prefixLength,
                            const QString &gateway,
                            const QStringList &dns,
                            QString *errorOut = nullptr);
bool applyRuntimeDhcp(const QString &iface, QString *errorOut = nullptr);
bool setLinkState(const QString &iface, bool up, QString *errorOut = nullptr);

bool persistMacAddress(const QString &iface, const QString &mac, QString *logOut = nullptr);
bool persistIpv4Config(const QString &iface,
                       bool useDhcp,
                       const QString &ip,
                       int prefixLength,
                       const QString &gateway,
                       const QStringList &dns,
                       QString *logOut = nullptr);

bool hasI210Nic();

} // namespace TbNet

#endif /* TB_NET_UTIL_H */

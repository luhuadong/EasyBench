#include "eb_net_util.h"

#include <QFile>
#include <QNetworkInterface>
#include <QRegExp>
#include <QtAlgorithms>
#include <QTimer>
#include <unistd.h>

namespace EbNet {

namespace {

QString readSysfsString(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll()).trimmed();
}

bool sysfsBool(const QString &path)
{
    const QString value = readSysfsString(path);
    return value == QLatin1String("1") || value == QLatin1String("up");
}

QString prefixToNetmask(int prefix)
{
    if (prefix <= 0 || prefix > 32) {
        return QString();
    }
    const quint32 mask = prefix == 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - prefix));
    return QStringLiteral("%1.%2.%3.%4")
        .arg((mask >> 24) & 0xFF)
        .arg((mask >> 16) & 0xFF)
        .arg((mask >> 8) & 0xFF)
        .arg(mask & 0xFF);
}

int netmaskToPrefix(const QString &netmask)
{
    const QStringList parts = netmask.split(QLatin1Char('.'));
    if (parts.size() != 4) {
        return -1;
    }
    quint32 mask = 0;
    for (const QString &part : parts) {
        mask = (mask << 8) | part.toUInt();
    }
    int prefix = 0;
    while (prefix < 32 && (mask & (1u << (31 - prefix)))) {
        prefix++;
    }
    return prefix;
}

QString lookupDefaultGateway(const QString &iface)
{
    const CommandResult route = runCommand(QStringLiteral("ip"),
                                           QStringList() << QStringLiteral("route")
                                                         << QStringLiteral("show")
                                                         << QStringLiteral("default")
                                                         << QStringLiteral("dev")
                                                         << iface);
    const QRegExp rx(QStringLiteral("default via ([0-9.]+)"));
    for (const QString &line : route.standardOutput.split(QLatin1Char('\n'))) {
        if (rx.indexIn(line) >= 0) {
            return rx.cap(1);
        }
    }
    return QString();
}

QStringList readDnsServers()
{
    QStringList servers;
    QFile resolv(QStringLiteral("/etc/resolv.conf"));
    if (resolv.open(QIODevice::ReadOnly)) {
        while (!resolv.atEnd()) {
            const QString line = QString::fromUtf8(resolv.readLine()).trimmed();
            if (line.startsWith(QStringLiteral("nameserver"))) {
                const QString ip = line.section(QLatin1Char(' '), 1).trimmed();
                if (!ip.isEmpty()) {
                    servers.append(ip);
                }
            }
        }
    }
    return servers;
}

QString nmcliConnectionForDevice(const QString &iface)
{
    const CommandResult result = runCommand(QStringLiteral("nmcli"),
                                            QStringList() << QStringLiteral("-t")
                                                          << QStringLiteral("-f")
                                                          << QStringLiteral("NAME,DEVICE")
                                                          << QStringLiteral("connection")
                                                          << QStringLiteral("show"));
    for (const QString &line : result.standardOutput.split(QLatin1Char('\n'))) {
        const QStringList parts = line.split(QLatin1Char(':'));
        if (parts.size() >= 2 && parts.at(1) == iface) {
            return parts.at(0);
        }
    }
    return QString();
}

} // namespace

QString backendName(NetworkBackend backend)
{
    switch (backend) {
    case NetworkBackend::NetworkManager:
        return QStringLiteral("NetworkManager");
    case NetworkBackend::Netplan:
        return QStringLiteral("Netplan");
    case NetworkBackend::Ifupdown:
        return QStringLiteral("/etc/network/interfaces");
    case NetworkBackend::IpRoute2:
        return QStringLiteral("iproute2");
    case NetworkBackend::Unknown:
    default:
        return QStringLiteral("Unknown");
    }
}

NetworkBackend detectNetworkBackend()
{
    if (runCommand(QStringLiteral("which"), QStringList() << QStringLiteral("nmcli")).exitCode == 0) {
        return NetworkBackend::NetworkManager;
    }
    if (QFile::exists(QStringLiteral("/etc/netplan"))) {
        return NetworkBackend::Netplan;
    }
    if (QFile::exists(QStringLiteral("/etc/network/interfaces"))) {
        return NetworkBackend::Ifupdown;
    }
    if (runCommand(QStringLiteral("which"), QStringList() << QStringLiteral("ip")).exitCode == 0) {
        return NetworkBackend::IpRoute2;
    }
    return NetworkBackend::Unknown;
}

bool hasNetAdminPrivilege()
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

QList<InterfaceInfo> listInterfaces()
{
    QList<InterfaceInfo> list;
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (!(iface.flags() & QNetworkInterface::IsUp)
            && !(iface.flags() & QNetworkInterface::IsRunning)) {
            // still include down interfaces for configuration
        }
        if (iface.flags() & QNetworkInterface::IsLoopBack) {
            continue;
        }
        InterfaceInfo info = readInterfaceConfig(iface.name());
        list.append(info);
    }
    qSort(list.begin(), list.end(),
          [](const InterfaceInfo &a, const InterfaceInfo &b) { return a.name < b.name; });
    return list;
}

InterfaceInfo readInterfaceConfig(const QString &ifaceName)
{
    InterfaceInfo info;
    info.name = ifaceName;
    info.operState = readSysfsString(QStringLiteral("/sys/class/net/%1/operstate").arg(ifaceName));
    info.linkUp = info.operState == QLatin1String("up") || info.operState == QLatin1String("unknown");
    info.carrier = sysfsBool(QStringLiteral("/sys/class/net/%1/carrier").arg(ifaceName));

    const QNetworkInterface iface = QNetworkInterface::interfaceFromName(ifaceName);
    if (iface.isValid()) {
        info.mac = iface.hardwareAddress();
        const QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                info.ipv4 = entry.ip().toString();
                info.prefixLength = entry.prefixLength();
                info.netmask = prefixToNetmask(info.prefixLength);
                break;
            }
        }
    }

    info.gateway = lookupDefaultGateway(ifaceName);
    info.dnsServers = readDnsServers();

    QString state = info.linkUp ? QStringLiteral("UP") : QStringLiteral("DOWN");
    if (!info.carrier && info.linkUp) {
        state += QStringLiteral(", no carrier");
    }
    info.displayText = info.ipv4.isEmpty()
        ? QStringLiteral("%1 (%2)").arg(ifaceName, state)
        : QStringLiteral("%1 (%2, %3)").arg(ifaceName, state, info.ipv4);
    return info;
}

bool isValidMac(const QString &mac)
{
    const QRegExp rx(QStringLiteral("^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$"));
    return rx.exactMatch(mac.trimmed());
}

QString normalizeMac(const QString &mac)
{
    return mac.trimmed().toLower();
}

bool applyRuntimeMac(const QString &iface, const QString &mac, QString *errorOut)
{
    if (!hasNetAdminPrivilege()) {
        if (errorOut) {
            *errorOut = QStringLiteral("需要 root 权限");
        }
        return false;
    }
    if (!isValidMac(mac)) {
        if (errorOut) {
            *errorOut = QStringLiteral("MAC 地址格式无效");
        }
        return false;
    }
    const QString m = normalizeMac(mac);
    QString log;
    log += runShell(QStringLiteral("ip link set dev %1 down").arg(iface));
    log += runShell(QStringLiteral("ip link set dev %1 address %2").arg(iface, m));
    log += runShell(QStringLiteral("ip link set dev %1 up").arg(iface));
    if (errorOut) {
        *errorOut = log.trimmed();
    }
    return true;
}

bool applyRuntimeIpv4Static(const QString &iface,
                            const QString &ip,
                            int prefixLength,
                            const QString &gateway,
                            const QStringList &dns,
                            QString *errorOut)
{
    if (!hasNetAdminPrivilege()) {
        if (errorOut) {
            *errorOut = QStringLiteral("需要 root 权限");
        }
        return false;
    }
    if (prefixLength <= 0 || prefixLength > 32) {
        if (errorOut) {
            *errorOut = QStringLiteral("无效子网前缀");
        }
        return false;
    }

    QString log;
    log += runShell(QStringLiteral("ip link set dev %1 up").arg(iface));
    log += runShell(QStringLiteral("ip addr flush dev %1").arg(iface));
    log += runShell(QStringLiteral("ip addr add %1/%2 dev %3").arg(ip).arg(prefixLength).arg(iface));
    if (!gateway.isEmpty()) {
        log += runShell(QStringLiteral("ip route replace default via %1 dev %2").arg(gateway, iface));
    }
    Q_UNUSED(dns);
    if (errorOut) {
        *errorOut = log.trimmed();
    }
    return true;
}

bool applyRuntimeDhcp(const QString &iface, QString *errorOut)
{
    if (!hasNetAdminPrivilege()) {
        if (errorOut) {
            *errorOut = QStringLiteral("需要 root 权限");
        }
        return false;
    }

    QString log;
    if (detectNetworkBackend() == NetworkBackend::NetworkManager) {
        log += runShell(QStringLiteral("nmcli dev disconnect %1 2>/dev/null; nmcli dev connect %1")
                            .arg(iface));
    } else if (runCommand(QStringLiteral("which"), QStringList() << QStringLiteral("dhclient")).exitCode
              == 0) {
        log += runShell(QStringLiteral("ip addr flush dev %1; dhclient -v %1").arg(iface));
    } else {
        log += runShell(QStringLiteral("ip link set dev %1 up").arg(iface));
        log += QStringLiteral("未找到 dhclient/nmcli，请手动获取 DHCP 地址");
    }
    if (errorOut) {
        *errorOut = log.trimmed();
    }
    return true;
}

bool setLinkState(const QString &iface, bool up, QString *errorOut)
{
    if (!hasNetAdminPrivilege()) {
        if (errorOut) {
            *errorOut = QStringLiteral("需要 root 权限");
        }
        return false;
    }
    const QString state = up ? QStringLiteral("up") : QStringLiteral("down");
    const QString log = runShell(QStringLiteral("ip link set dev %1 %2").arg(iface, state));
    if (errorOut) {
        *errorOut = log.trimmed();
    }
    return true;
}

bool persistMacAddress(const QString &iface, const QString &mac, QString *logOut)
{
    if (!hasNetAdminPrivilege()) {
        if (logOut) {
            *logOut = QStringLiteral("需要 root 权限才能保存 MAC");
        }
        return false;
    }
    if (!isValidMac(mac)) {
        if (logOut) {
            *logOut = QStringLiteral("MAC 格式无效");
        }
        return false;
    }

    QString log;
    const NetworkBackend backend = detectNetworkBackend();
    const QString normalized = normalizeMac(mac);

    if (backend == NetworkBackend::NetworkManager) {
        const QString conn = nmcliConnectionForDevice(iface);
        if (!conn.isEmpty()) {
            log += runShell(QStringLiteral("nmcli connection modify '%1' ethernet.cloned-mac-address %2")
                                .arg(conn, normalized));
            log += runShell(QStringLiteral("nmcli connection down '%1'; nmcli connection up '%1'")
                                .arg(conn));
        } else {
            log += QStringLiteral("未找到 %1 的 NetworkManager 连接\n").arg(iface);
        }
    }

    const QString udevPath =
        QStringLiteral("/etc/udev/rules.d/99-easybench-%1-mac.rules").arg(iface);
    const QString udevRule =
        QStringLiteral("SUBSYSTEM==\"net\", ACTION==\"add\", KERNEL==\"%1\", "
                       "RUN+=\"/sbin/ip link set dev %1 address %2\"\n")
            .arg(iface, normalized);
    QFile udevFile(udevPath);
    if (udevFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        udevFile.write(udevRule.toUtf8());
        udevFile.close();
        log += QStringLiteral("已写入 %1\n").arg(udevPath);
    } else {
        log += QStringLiteral("无法写入 %1\n").arg(udevPath);
    }

    if (logOut) {
        *logOut = log.trimmed();
    }
    return true;
}

bool persistIpv4Config(const QString &iface,
                       bool useDhcp,
                       const QString &ip,
                       int prefixLength,
                       const QString &gateway,
                       const QStringList &dns,
                       QString *logOut)
{
    if (!hasNetAdminPrivilege()) {
        if (logOut) {
            *logOut = QStringLiteral("需要 root 权限才能保存网络配置");
        }
        return false;
    }

    QString log;
    const NetworkBackend backend = detectNetworkBackend();

    if (backend == NetworkBackend::NetworkManager) {
        const QString conn = nmcliConnectionForDevice(iface);
        if (!conn.isEmpty()) {
            if (useDhcp) {
                log += runShell(QStringLiteral(
                    "nmcli connection modify '%1' ipv4.method auto ipv4.addresses ''").arg(conn));
            } else {
                const QString dnsJoined = dns.join(QLatin1Char(','));
                log += runShell(QStringLiteral(
                                    "nmcli connection modify '%1' ipv4.method manual "
                                    "ipv4.addresses %2/%3 ipv4.gateway %4 ipv4.dns '%5'")
                                    .arg(conn, ip)
                                    .arg(prefixLength)
                                    .arg(gateway, dnsJoined));
            }
            log += runShell(QStringLiteral("nmcli connection down '%1'; nmcli connection up '%1'")
                                .arg(conn));
        } else {
            log += QStringLiteral("未找到 %1 的 NM 连接\n").arg(iface);
        }
    } else if (backend == NetworkBackend::Ifupdown || backend == NetworkBackend::Netplan) {
        const QString path =
            QStringLiteral("/etc/network/interfaces.d/easybench-%1.cfg").arg(iface);
        QString content;
        if (useDhcp) {
            content = QStringLiteral("auto %1\niface %1 inet dhcp\n").arg(iface);
        } else {
            const QString mask = prefixToNetmask(prefixLength);
            content = QStringLiteral("auto %1\niface %1 inet static\n"
                                      "    address %2\n"
                                      "    netmask %3\n"
                                      "    gateway %4\n")
                              .arg(iface, ip, mask, gateway);
            if (!dns.isEmpty()) {
                content += QStringLiteral("    dns-nameservers %1\n").arg(dns.join(QLatin1Char(' ')));
            }
        }
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(content.toUtf8());
            file.close();
            log += QStringLiteral("已写入 %1\n").arg(path);
            if (backend == NetworkBackend::Ifupdown) {
                log += runShell(QStringLiteral("ifup %1 2>/dev/null || true").arg(iface));
            }
        } else {
            log += QStringLiteral("无法写入 %1（请检查权限）\n").arg(path);
        }
        if (backend == NetworkBackend::Netplan) {
            log += QStringLiteral(
                "检测到 Netplan：建议手动编辑 /etc/netplan/*.yaml 后执行 netplan apply\n");
        }
    } else {
        log += QStringLiteral("未检测到可持久化的网络后端，仅可使用「应用（临时）」\n");
    }

    if (logOut) {
        *logOut = log.trimmed();
    }
    return !log.isEmpty();
}

bool hasI210Nic()
{
    const CommandResult result = runCommand(QStringLiteral("lspci"), QStringList());
    return result.standardOutput.contains(QStringLiteral("I210"));
}

PingRunner::PingRunner(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &PingRunner::onReadyRead);
    connect(m_process, &QProcess::readyReadStandardError, this, &PingRunner::onReadyRead);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &PingRunner::onFinished);
}

void PingRunner::start(const QString &host, int count)
{
    stop();
    m_process->start(QStringLiteral("ping"),
                     QStringList() << QStringLiteral("-c") << QString::number(qMax(1, count))
                                   << host);
}

void PingRunner::stop()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(2000)) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
}

void PingRunner::onReadyRead()
{
    const QByteArray out = m_process->readAllStandardOutput();
    const QByteArray err = m_process->readAllStandardError();
    const QString text = QString::fromUtf8(out + err);
    for (const QString &line : text.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        emit lineOutput(line);
    }
}

void PingRunner::onFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    emit finished(exitCode);
}

} // namespace EbNet

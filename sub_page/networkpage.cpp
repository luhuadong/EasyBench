#include "networkpage.h"

#include <QFile>
#include <QRegExp>
#include <QButtonGroup>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

namespace {

void setInfoLine(QLabel *label, const QString &name, const QString &value)
{
    label->setText(QStringLiteral("%1：%2").arg(name, value));
}

int prefixFromMaskField(const QString &mask, int fallback)
{
    const QStringList parts = mask.split(QLatin1Char('.'));
    if (parts.size() != 4) {
        return fallback;
    }
    quint32 value = 0;
    for (const QString &part : parts) {
        value = (value << 8) | part.toUInt();
    }
    int prefix = 0;
    while (prefix < 32 && (value & (1u << (31 - prefix)))) {
        prefix++;
    }
    return prefix > 0 ? prefix : fallback;
}

} // namespace

NetworkPage::NetworkPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("网络配置与测试"));
    pingRunner = new EbNet::PingRunner(this);
    connect(pingRunner, &EbNet::PingRunner::lineOutput, this, &NetworkPage::onPingLine);
    connect(pingRunner, &EbNet::PingRunner::finished, this, &NetworkPage::onPingFinished);

    buildUi();
    refreshInterfaces();
}

void NetworkPage::toggleInterfaceLink()
{
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        return;
    }
    const EbNet::InterfaceInfo info = EbNet::readInterfaceConfig(iface);
    if (info.linkUp) {
        setInterfaceDown();
    } else {
        setInterfaceUp();
    }
}

void NetworkPage::syncLinkToggleButton()
{
    if (!linkToggleBtn) {
        return;
    }
    linkToggleBtn->setEnabled(EbNet::hasNetAdminPrivilege() && !currentInterfaceName().isEmpty());
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        linkToggleBtn->setText(tr("接口 Up"));
        return;
    }
    const EbNet::InterfaceInfo info = EbNet::readInterfaceConfig(iface);
    linkToggleBtn->setText(info.linkUp ? tr("接口 Down") : tr("接口 Up"));
}

void NetworkPage::buildUi()
{
    QWidget *content = contentArea();

    QWidget *toolbar = new QWidget(content);
    toolbar->setObjectName(QStringLiteral("networkToolbar"));
    ifaceBox = new QComboBox(toolbar);
    ifaceBox->setMinimumWidth(240);
    QPushButton *refreshBtn = new QPushButton(tr("刷新"), toolbar);
    refreshBtn->setObjectName(QStringLiteral("functionBtn_small"));
    linkToggleBtn = new QPushButton(tr("接口 Up"), toolbar);
    linkToggleBtn->setObjectName(QStringLiteral("functionBtn_small"));
    connect(linkToggleBtn, &QPushButton::clicked, this, &NetworkPage::toggleInterfaceLink);

    QHBoxLayout *toolbarRow = new QHBoxLayout(toolbar);
    toolbarRow->setContentsMargins(16, 8, 16, 8);
    toolbarRow->setSpacing(8);
    toolbarRow->addWidget(ifaceBox, 1);
    toolbarRow->addWidget(refreshBtn);
    toolbarRow->addWidget(linkToggleBtn);
    toolbar->setLayout(toolbarRow);

    tabWidget = new QTabWidget(content);
    tabWidget->setObjectName(QStringLiteral("networkTabs"));
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *statusTab = new QWidget(tabWidget);
    statusMacLabel = new QLabel(statusTab);
    statusIpLabel = new QLabel(statusTab);
    statusGwLabel = new QLabel(statusTab);
    statusDnsLabel = new QLabel(statusTab);
    statusLinkLabel = new QLabel(statusTab);
    statusBackendLabel = new QLabel(statusTab);
    const QList<QLabel *> statusLabels = {statusMacLabel, statusIpLabel, statusGwLabel,
                                          statusDnsLabel, statusLinkLabel, statusBackendLabel};
    for (QLabel *label : statusLabels) {
        label->setWordWrap(true);
    }

    QVBoxLayout *statusLeft = new QVBoxLayout;
    statusLeft->setSpacing(8);
    statusLeft->addWidget(statusMacLabel);
    statusLeft->addWidget(statusIpLabel);
    statusLeft->addWidget(statusGwLabel);
    QVBoxLayout *statusRight = new QVBoxLayout;
    statusRight->setSpacing(8);
    statusRight->addWidget(statusDnsLabel);
    statusRight->addWidget(statusLinkLabel);
    statusRight->addWidget(statusBackendLabel);
    statusRight->addStretch();
    statusLeft->addStretch();

    QHBoxLayout *statusCols = new QHBoxLayout(statusTab);
    statusCols->setContentsMargins(16, 12, 16, 12);
    statusCols->addLayout(statusLeft, 1);
    statusCols->addLayout(statusRight, 1);
    tabWidget->addTab(statusTab, tr("状态"));

    QWidget *macTab = new QWidget(tabWidget);
    QGroupBox *macGroup = new QGroupBox(tr("MAC 地址"), macTab);
    macEdit = new QLineEdit(macGroup);
    macEdit->setObjectName(QStringLiteral("inputLineEdit"));
    macEdit->setPlaceholderText(QStringLiteral("00:11:22:33:44:55"));
    macApplyBtn = new QPushButton(tr("应用（临时）"), macGroup);
    macApplyBtn->setObjectName(QStringLiteral("functionBtn_small"));
    macSaveBtn = new QPushButton(tr("保存 MAC"), macGroup);
    macSaveBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QHBoxLayout *macBtnRow = new QHBoxLayout;
    macBtnRow->addWidget(macApplyBtn);
    macBtnRow->addWidget(macSaveBtn);
    macBtnRow->addStretch();
    QFormLayout *macForm = new QFormLayout(macGroup);
    macForm->setContentsMargins(12, 16, 12, 12);
    macForm->addRow(tr("新 MAC"), macEdit);
    macForm->addRow(QString(), macBtnRow);

    macLogArea = new QTextEdit(macTab);
    macLogArea->setReadOnly(true);
    macLogArea->setObjectName(QStringLiteral("networkLogArea"));
    QVBoxLayout *macLayout = new QVBoxLayout(macTab);
    macLayout->setContentsMargins(8, 8, 8, 8);
    macLayout->addWidget(macGroup);
    macLayout->addWidget(macLogArea, 1);
    tabWidget->addTab(macTab, tr("MAC"));

    QWidget *ipv4Tab = new QWidget(tabWidget);
    QGroupBox *ipGroup = new QGroupBox(tr("IPv4 配置"), ipv4Tab);
    dhcpRadio = new QRadioButton(tr("DHCP 自动获取"), ipGroup);
    staticRadio = new QRadioButton(tr("静态 IP"), ipGroup);
    staticRadio->setChecked(true);
    QButtonGroup *modeGroup = new QButtonGroup(ipGroup);
    modeGroup->addButton(dhcpRadio);
    modeGroup->addButton(staticRadio);

    ipEdit = new QLineEdit(ipGroup);
    ipEdit->setObjectName(QStringLiteral("inputLineEdit"));
    maskEdit = new QLineEdit(ipGroup);
    maskEdit->setObjectName(QStringLiteral("inputLineEdit"));
    maskEdit->setPlaceholderText(QStringLiteral("255.255.255.0"));
    prefixSpin = new QSpinBox(ipGroup);
    prefixSpin->setRange(1, 32);
    prefixSpin->setValue(24);
    gatewayEdit = new QLineEdit(ipGroup);
    gatewayEdit->setObjectName(QStringLiteral("inputLineEdit"));
    dnsEdit = new QLineEdit(ipGroup);
    dnsEdit->setObjectName(QStringLiteral("inputLineEdit"));
    dnsEdit->setPlaceholderText(QStringLiteral("8.8.8.8, 114.114.114.114"));

    ipApplyBtn = new QPushButton(tr("应用（临时）"), ipGroup);
    ipApplyBtn->setObjectName(QStringLiteral("functionBtn_small"));
    ipSaveBtn = new QPushButton(tr("保存网络"), ipGroup);
    ipSaveBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QHBoxLayout *ipBtnRow = new QHBoxLayout;
    ipBtnRow->addWidget(ipApplyBtn);
    ipBtnRow->addWidget(ipSaveBtn);
    ipBtnRow->addStretch();

    QFormLayout *ipForm = new QFormLayout(ipGroup);
    ipForm->setContentsMargins(12, 16, 12, 12);
    ipForm->addRow(QString(), dhcpRadio);
    ipForm->addRow(QString(), staticRadio);
    ipForm->addRow(tr("IP 地址"), ipEdit);
    ipForm->addRow(tr("子网掩码"), maskEdit);
    ipForm->addRow(tr("前缀长度"), prefixSpin);
    ipForm->addRow(tr("默认网关"), gatewayEdit);
    ipForm->addRow(tr("DNS"), dnsEdit);
    ipForm->addRow(QString(), ipBtnRow);

    ipv4LogArea = new QTextEdit(ipv4Tab);
    ipv4LogArea->setReadOnly(true);
    ipv4LogArea->setObjectName(QStringLiteral("networkLogArea"));
    QVBoxLayout *ipv4Layout = new QVBoxLayout(ipv4Tab);
    ipv4Layout->setContentsMargins(8, 8, 8, 8);
    ipv4Layout->addWidget(ipGroup);
    ipv4Layout->addWidget(ipv4LogArea, 1);
    tabWidget->addTab(ipv4Tab, tr("IPv4"));

    QWidget *pingTab = new QWidget(tabWidget);
    QGroupBox *pingGroup = new QGroupBox(tr("Ping 测试"), pingTab);
    pingHostEdit = new QLineEdit(pingGroup);
    pingHostEdit->setObjectName(QStringLiteral("inputLineEdit"));
    pingHostEdit->setText(QStringLiteral("8.8.8.8"));
    pingCountSpin = new QSpinBox(pingGroup);
    pingCountSpin->setRange(1, 20);
    pingCountSpin->setValue(4);
    pingBtn = new QPushButton(tr("Ping"), pingGroup);
    pingBtn->setObjectName(QStringLiteral("functionBtn_small"));
    pingStopBtn = new QPushButton(tr("停止"), pingGroup);
    pingStopBtn->setObjectName(QStringLiteral("functionBtn_small"));
    pingStopBtn->setEnabled(false);
    pingSummaryLabel = new QLabel(pingGroup);
    pingSummaryLabel->setWordWrap(true);
    QHBoxLayout *pingBtnRow = new QHBoxLayout;
    pingBtnRow->addWidget(pingBtn);
    pingBtnRow->addWidget(pingStopBtn);
    pingBtnRow->addStretch();
    QFormLayout *pingForm = new QFormLayout(pingGroup);
    pingForm->setContentsMargins(12, 16, 12, 12);
    pingForm->addRow(tr("目标"), pingHostEdit);
    pingForm->addRow(tr("次数"), pingCountSpin);
    pingForm->addRow(QString(), pingBtnRow);
    pingForm->addRow(tr("摘要"), pingSummaryLabel);

    pingLogArea = new QTextEdit(pingTab);
    pingLogArea->setReadOnly(true);
    pingLogArea->setObjectName(QStringLiteral("networkLogArea"));
    QVBoxLayout *pingLayout = new QVBoxLayout(pingTab);
    pingLayout->setContentsMargins(8, 8, 8, 8);
    pingLayout->addWidget(pingGroup);
    pingLayout->addWidget(pingLogArea, 1);
    tabWidget->addTab(pingTab, tr("连通性"));

    QWidget *i210Tab = new QWidget(tabWidget);
    buildI210Tab(i210Tab);
    i210TabIndex = tabWidget->addTab(i210Tab, tr("高级 I210"));
    if (!EbNet::hasI210Nic()) {
        tabWidget->setTabEnabled(i210TabIndex, false);
        tabWidget->setTabToolTip(i210TabIndex, tr("未检测到 Intel I210 网卡"));
    }

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(toolbar);
    pageLayout->addWidget(tabWidget, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &NetworkPage::refreshInterfaces);
    connect(ifaceBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &NetworkPage::onInterfaceChanged);
    connect(macApplyBtn, &QPushButton::clicked, this, &NetworkPage::applyMacRuntime);
    connect(macSaveBtn, &QPushButton::clicked, this, &NetworkPage::saveMacPersistent);
    connect(ipApplyBtn, &QPushButton::clicked, this, &NetworkPage::applyIpv4Runtime);
    connect(ipSaveBtn, &QPushButton::clicked, this, &NetworkPage::saveIpv4Persistent);
    connect(pingBtn, &QPushButton::clicked, this, &NetworkPage::startPing);
    connect(pingStopBtn, &QPushButton::clicked, this, &NetworkPage::stopPing);

    applyPrivilegeUi();
}

void NetworkPage::buildI210Tab(QWidget *tabPage)
{
    QGroupBox *i210Group = new QGroupBox(tr("Intel I210 EEPROM（产线）"), tabPage);

    i210MacLine = new QLineEdit(i210Group);
    i210MacLine->setObjectName(QStringLiteral("inputLineEdit"));
    i210MacLine->setInputMask(QStringLiteral("HH:HH:HH:HH:HH:HH;0"));
    i210MacLine->setReadOnly(true);
    i210MacLine->setText(QStringLiteral("00:25:A8:01:7D:B6"));

    QPushButton *editBtn = new QPushButton(tr("编辑"), i210Group);
    editBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QPushButton *checkBtn = new QPushButton(tr("检查"), i210Group);
    checkBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QPushButton *dumpBtn = new QPushButton(tr("读取"), i210Group);
    dumpBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QPushButton *writeBtn = new QPushButton(tr("写入"), i210Group);
    writeBtn->setObjectName(QStringLiteral("functionBtn_small"));

    QHBoxLayout *i210BtnRow = new QHBoxLayout;
    i210BtnRow->addWidget(editBtn);
    i210BtnRow->addWidget(checkBtn);
    i210BtnRow->addWidget(dumpBtn);
    i210BtnRow->addWidget(writeBtn);
    i210BtnRow->addStretch();

    QFormLayout *i210Form = new QFormLayout(i210Group);
    i210Form->setContentsMargins(12, 16, 12, 12);
    i210Form->addRow(tr("MAC"), i210MacLine);
    i210Form->addRow(QString(), i210BtnRow);

    i210LogArea = new QTextEdit(tabPage);
    i210LogArea->setReadOnly(true);
    i210LogArea->setObjectName(QStringLiteral("networkLogArea"));

    QVBoxLayout *i210Layout = new QVBoxLayout(tabPage);
    i210Layout->setContentsMargins(8, 8, 8, 8);
    i210Layout->addWidget(i210Group);
    i210Layout->addWidget(i210LogArea, 1);

    connect(editBtn, &QPushButton::clicked, this, &NetworkPage::i210EditClicked);
    connect(checkBtn, &QPushButton::clicked, this, &NetworkPage::i210CheckClicked);
    connect(dumpBtn, &QPushButton::clicked, this, &NetworkPage::i210DumpClicked);
    connect(writeBtn, &QPushButton::clicked, this, &NetworkPage::i210WriteClicked);
}

void NetworkPage::appendMacLog(const QString &text)
{
    if (macLogArea) {
        macLogArea->append(text);
    }
}

void NetworkPage::appendIpv4Log(const QString &text)
{
    if (ipv4LogArea) {
        ipv4LogArea->append(text);
    }
}

void NetworkPage::appendPingLog(const QString &text)
{
    if (pingLogArea) {
        pingLogArea->append(text);
    }
}

QString NetworkPage::defaultStatusHint() const
{
    if (!EbNet::hasNetAdminPrivilege()) {
        return tr("提示：当前非 root，修改 MAC/IPv4 与接口 up/down 不可用；Ping 仍可用。");
    }
    return tr("已具备网络管理权限（root）。");
}

void NetworkPage::applyPrivilegeUi()
{
    setStatusMessage(defaultStatusHint());
    const bool canAdmin = EbNet::hasNetAdminPrivilege();
    if (macApplyBtn) {
        macApplyBtn->setEnabled(canAdmin);
    }
    if (macSaveBtn) {
        macSaveBtn->setEnabled(canAdmin);
    }
    if (ipApplyBtn) {
        ipApplyBtn->setEnabled(canAdmin);
    }
    if (ipSaveBtn) {
        ipSaveBtn->setEnabled(canAdmin);
    }
    syncLinkToggleButton();
}

bool NetworkPage::confirmNetworkChange(const QString &title, const QString &detail)
{
    return QMessageBox::question(this, title, detail, QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No)
           == QMessageBox::Yes;
}

QString NetworkPage::currentInterfaceName() const
{
    const int index = ifaceBox->currentIndex();
    if (index < 0 || index >= interfaces.size()) {
        return QString();
    }
    return interfaces.at(index).name;
}

void NetworkPage::refreshInterfaces()
{
    const QString previous = currentInterfaceName();
    interfaces = EbNet::listInterfaces();
    ifaceBox->blockSignals(true);
    ifaceBox->clear();
    int restoreIndex = 0;
    for (int i = 0; i < interfaces.size(); ++i) {
        const EbNet::InterfaceInfo &info = interfaces.at(i);
        ifaceBox->addItem(info.displayText, info.name);
        if (info.name == previous) {
            restoreIndex = i;
        }
    }
    ifaceBox->blockSignals(false);
    if (ifaceBox->count() > 0) {
        ifaceBox->setCurrentIndex(restoreIndex);
        onInterfaceChanged(restoreIndex);
    } else {
        appendMacLog(tr("未找到可用网卡（已排除 loopback）。"));
    }
    syncLinkToggleButton();
}

void NetworkPage::onInterfaceChanged(int index)
{
    if (index < 0 || index >= interfaces.size()) {
        return;
    }
    const EbNet::InterfaceInfo info = EbNet::readInterfaceConfig(interfaces.at(index).name);
    interfaces[index] = info;
    updateStatusLabels();
    fillConfigFields(info);
}

void NetworkPage::updateStatusLabels()
{
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        return;
    }
    const EbNet::InterfaceInfo info = EbNet::readInterfaceConfig(iface);
    setInfoLine(statusMacLabel, tr("MAC"), info.mac.isEmpty() ? tr("—") : info.mac);
    const QString ipText = info.ipv4.isEmpty()
        ? tr("—")
        : QStringLiteral("%1 / %2").arg(info.ipv4).arg(info.prefixLength);
    setInfoLine(statusIpLabel, tr("IPv4"), ipText);
    setInfoLine(statusGwLabel, tr("网关"), info.gateway.isEmpty() ? tr("—") : info.gateway);
    setInfoLine(statusDnsLabel, tr("DNS"), info.dnsServers.isEmpty() ? tr("—")
                                                                    : info.dnsServers.join(QLatin1Char(
                                                                          ' ')));
    setInfoLine(statusLinkLabel, tr("链路"),
                QStringLiteral("%1, carrier=%2")
                    .arg(info.operState.isEmpty() ? (info.linkUp ? tr("up") : tr("down"))
                                                   : info.operState)
                    .arg(info.carrier ? tr("有") : tr("无")));
    setInfoLine(statusBackendLabel, tr("配置后端"),
                EbNet::backendName(EbNet::detectNetworkBackend()));
    syncLinkToggleButton();
}

void NetworkPage::fillConfigFields(const EbNet::InterfaceInfo &info)
{
    macEdit->setText(info.mac);
    ipEdit->setText(info.ipv4);
    maskEdit->setText(info.netmask);
    if (info.prefixLength > 0) {
        prefixSpin->setValue(info.prefixLength);
    }
    gatewayEdit->setText(info.gateway);
    dnsEdit->setText(info.dnsServers.join(QLatin1Char(',')));
}

bool NetworkPage::checkPrivilegeForApply()
{
    if (EbNet::hasNetAdminPrivilege()) {
        return true;
    }
    QMessageBox::warning(this, tr("权限不足"),
                         tr("修改网络配置需要 root 权限，请使用 sudo 运行 EasyBench。"));
    return false;
}

void NetworkPage::applyMacRuntime()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    const QString iface = currentInterfaceName();
    const QString mac = macEdit->text().trimmed();
    if (!confirmNetworkChange(tr("应用 MAC"),
                              tr("将临时修改 %1 的 MAC 为 %2，重启后可能恢复。\n是否继续？")
                                  .arg(iface, mac))) {
        return;
    }
    QString log;
    const bool ok = EbNet::applyRuntimeMac(iface, mac, &log);
    appendMacLog(tr("--- 应用 MAC ---"));
    appendMacLog(log);
    refreshInterfaces();
    syncLinkToggleButton();
    if (!ok) {
        QMessageBox::warning(this, tr("MAC"), tr("应用失败，详见日志。"));
    }
}

void NetworkPage::saveMacPersistent()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    const QString iface = currentInterfaceName();
    const QString mac = macEdit->text().trimmed();
    if (!confirmNetworkChange(tr("保存 MAC"),
                              tr("将尝试写入系统持久配置（NM/udev）。\n网卡：%1\nMAC：%2\n是否继续？")
                                  .arg(iface, mac))) {
        return;
    }
    QString log;
    EbNet::persistMacAddress(iface, mac, &log);
    appendMacLog(tr("--- 保存 MAC ---"));
    appendMacLog(log);
}

void NetworkPage::applyIpv4Runtime()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    const QString iface = currentInterfaceName();
    QString log;
    bool ok = false;
    if (dhcpRadio->isChecked()) {
        if (!confirmNetworkChange(tr("应用 DHCP"), tr("将为 %1 尝试 DHCP（临时）。\n是否继续？").arg(iface))) {
            return;
        }
        ok = EbNet::applyRuntimeDhcp(iface, &log);
    } else {
        const int prefix = prefixFromMaskField(maskEdit->text().trimmed(), prefixSpin->value());
        const QStringList dns =
            dnsEdit->text().split(QRegExp(QStringLiteral("[,\\s]+")), QString::SkipEmptyParts);
        if (!confirmNetworkChange(
                tr("应用静态 IP"),
                tr("将为 %1 设置临时静态地址 %2/%3\n是否继续？").arg(iface, ipEdit->text()).arg(prefix))) {
            return;
        }
        ok = EbNet::applyRuntimeIpv4Static(iface, ipEdit->text().trimmed(), prefix,
                                           gatewayEdit->text().trimmed(), dns, &log);
    }
    appendIpv4Log(tr("--- 应用 IPv4 ---"));
    appendIpv4Log(log);
    refreshInterfaces();
    if (!ok) {
        QMessageBox::warning(this, tr("IPv4"), tr("应用可能未完全成功，详见日志。"));
    }
}

void NetworkPage::saveIpv4Persistent()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    const QString iface = currentInterfaceName();
    if (!confirmNetworkChange(tr("保存网络"),
                              tr("将写入系统持久配置，可能覆盖现有网络设置。\n是否继续？"))) {
        return;
    }
    const int prefix = prefixFromMaskField(maskEdit->text().trimmed(), prefixSpin->value());
    const QStringList dns =
        dnsEdit->text().split(QRegExp(QStringLiteral("[,\\s]+")), QString::SkipEmptyParts);
    QString log;
    EbNet::persistIpv4Config(iface, dhcpRadio->isChecked(), ipEdit->text().trimmed(), prefix,
                             gatewayEdit->text().trimmed(), dns, &log);
    appendIpv4Log(tr("--- 保存网络 ---"));
    appendIpv4Log(log);
}

void NetworkPage::setInterfaceUp()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    QString log;
    EbNet::setLinkState(currentInterfaceName(), true, &log);
    appendMacLog(log);
    refreshInterfaces();
    syncLinkToggleButton();
}

void NetworkPage::setInterfaceDown()
{
    if (!checkPrivilegeForApply()) {
        return;
    }
    QString log;
    EbNet::setLinkState(currentInterfaceName(), false, &log);
    appendMacLog(log);
    refreshInterfaces();
    syncLinkToggleButton();
}

void NetworkPage::startPing()
{
    const QString host = pingHostEdit->text().trimmed();
    if (host.isEmpty()) {
        QMessageBox::information(this, tr("Ping"), tr("请输入目标地址。"));
        return;
    }
    pingSummaryLabel->clear();
    setStatusMessage(tr("正在 Ping %1 …").arg(host));
    appendPingLog(tr("--- Ping %1 ---").arg(host));
    pingBtn->setEnabled(false);
    pingStopBtn->setEnabled(true);
    pingRunner->start(host, pingCountSpin->value());
}

void NetworkPage::stopPing()
{
    pingRunner->stop();
    pingBtn->setEnabled(true);
    pingStopBtn->setEnabled(false);
}

void NetworkPage::onPingLine(const QString &line)
{
    appendPingLog(line);
    if (line.contains(QStringLiteral("packet loss")) || line.contains(QStringLiteral("丢包"))) {
        pingSummaryLabel->setText(line);
    }
}

void NetworkPage::onPingFinished(int exitCode)
{
    pingBtn->setEnabled(true);
    pingStopBtn->setEnabled(false);
    pingSummaryLabel->setText(tr("Ping 结束，退出码 %1").arg(exitCode));
    appendPingLog(tr("Ping 结束，退出码 %1").arg(exitCode));
    setStatusMessage(defaultStatusHint());
}

QString NetworkPage::parseI210MacLine()
{
    const QString macStr = i210MacLine->text();
    i210MacParts = macStr.split(QLatin1Char(':'));
    for (int i = 0; i < i210MacParts.count() && i < 6; ++i) {
        QString part = i210MacParts.at(i);
        part = QStringLiteral("%1").arg(part.toInt(0, 16), 2, 16, QLatin1Char('0')).toUpper();
        i210MacParts.replace(i, part);
        i210MacBytes[i] = i210MacParts.at(i).isEmpty() ? 0 : static_cast<char>(part.toUShort(0, 16));
    }
    return i210MacParts.join(QLatin1Char(':'));
}

void NetworkPage::i210EditClicked()
{
    i210MacLine->setReadOnly(false);
    i210MacLine->setFocus();
}

void NetworkPage::i210CheckClicked()
{
    const EbNet::CommandResult result =
        EbNet::runCommand(QStringLiteral("./eepromARMtool"), QStringList());
    i210LogArea->clear();
    if (result.exitCode != 0 && !result.standardOutput.isEmpty()) {
        i210LogArea->setText(result.standardError.isEmpty() ? result.standardOutput
                                                            : result.standardError);
        return;
    }
    for (const QString &line : result.standardOutput.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        i210LogArea->append(line);
    }
}

void NetworkPage::i210DumpClicked()
{
    const EbNet::CommandResult result = EbNet::runCommand(
        QStringLiteral("./eepromARMtool"), QStringList() << QStringLiteral("-dump") << QStringLiteral("-NIC=1"));
    i210LogArea->clear();
    for (const QString &line : result.standardOutput.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        i210LogArea->append(line);
    }
    i210LogArea->append(QStringLiteral("------------------------------------------------"));
    QFile file(QStringLiteral("./I210NIC.otp"));
    if (file.open(QIODevice::ReadOnly)) {
        i210LogArea->append(QString::fromUtf8(file.readAll()));
        file.close();
    }
}

bool NetworkPage::generateI210File()
{
    QMessageBox msgBox(this);
    QPushButton *noBtn = msgBox.addButton(tr("否"), QMessageBox::RejectRole);
    QPushButton *yesBtn = msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
    msgBox.setText(tr("MAC 地址：%1\n确认写入 EEPROM？").arg(parseI210MacLine()));
    msgBox.exec();
    if (msgBox.clickedButton() != yesBtn) {
        return false;
    }

    QFile i210File(QStringLiteral("I210NIC-origin.otp"));
    if (!i210File.exists()) {
        i210LogArea->setText(tr("错误：找不到 I210NIC-origin.otp"));
        return false;
    }
    if (!i210File.open(QIODevice::ReadOnly)) {
        i210LogArea->setText(tr("错误：无法打开 I210NIC-origin.otp"));
        return false;
    }

    i210TmpFile.setFileName(QStringLiteral("tmp.otp"));
    if (!i210TmpFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        i210LogArea->setText(tr("错误：无法创建 tmp.otp"));
        return false;
    }

    int flag = 0;
    while (!i210File.atEnd()) {
        QString line = QString::fromUtf8(i210File.readLine());
        if (flag == 0) {
            QStringList parts = line.split(QLatin1Char(' '), QString::SkipEmptyParts);
            if (parts.size() >= 3) {
                parts[0] = i210MacParts.at(1) + i210MacParts.at(0);
                parts[1] = i210MacParts.at(3) + i210MacParts.at(2);
                parts[2] = i210MacParts.at(5) + i210MacParts.at(4);
                line = parts.join(QLatin1Char(' ')) + QLatin1Char('\n');
            }
        }
        flag++;
        i210TmpFile.write(line.toLocal8Bit());
    }
    i210File.close();
    i210TmpFile.close();
    return true;
}

void NetworkPage::i210WriteClicked()
{
    if (!generateI210File()) {
        return;
    }
    const QString cmd =
        QStringLiteral("./eepromARMtool -write -NIC=1 -f=%1").arg(i210TmpFile.fileName());
    const EbNet::CommandResult result =
        EbNet::runCommand(QStringLiteral("/bin/sh"), QStringList() << QStringLiteral("-c") << cmd);
    i210LogArea->clear();
    for (const QString &line : result.standardOutput.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        i210LogArea->append(line);
    }
    if (result.standardOutput.contains(QStringLiteral("Flash update complete"))) {
        QMessageBox msg(this);
        msg.setText(tr("烧录完成。是否立即重启？"));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msg.exec() == QMessageBox::Yes) {
            EbNet::runShell(QStringLiteral("reboot"));
        }
    }
}

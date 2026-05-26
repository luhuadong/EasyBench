#include "networkpage.h"
#include "widgets/tb_widget_util.h"
#include "tb_paths.h"

#include <QFile>
#include <QRegExp>
#include <QButtonGroup>
#include <QCheckBox>
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

NetworkPage::NetworkPage(TbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("网络配置与测试"));
    pingRunner = new TbNet::PingRunner(this);
    connect(pingRunner, &TbNet::PingRunner::lineOutput, this, &NetworkPage::onPingLine);
    connect(pingRunner, &TbNet::PingRunner::finished, this, &NetworkPage::onPingFinished);

    buildUi();
    refreshInterfaces();
}

void NetworkPage::toggleInterfaceLink()
{
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        return;
    }
    const TbNet::InterfaceInfo info = TbNet::readInterfaceConfig(iface);
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
    linkToggleBtn->setEnabled(TbNet::hasNetAdminPrivilege() && !currentInterfaceName().isEmpty());
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        linkToggleBtn->setText(tr("接口 Up"));
        return;
    }
    const TbNet::InterfaceInfo info = TbNet::readInterfaceConfig(iface);
    linkToggleBtn->setText(info.linkUp ? tr("接口 Down") : tr("接口 Up"));
}

void NetworkPage::buildUi()
{
    QWidget *content = contentArea();

    QWidget *toolbar = new QWidget(content);
    toolbar->setObjectName(QStringLiteral("networkToolbar"));
    ifaceBox = new QComboBox(toolbar);
    ifaceBox->setMinimumWidth(200);
    TbWidget::applyComboBoxStyle(ifaceBox);
    QPushButton *refreshBtn = new QPushButton(tr("刷新"), toolbar);
    refreshBtn->setObjectName(QStringLiteral("functionBtn_small"));
    linkToggleBtn = new QPushButton(tr("接口 Up"), toolbar);
    linkToggleBtn->setObjectName(QStringLiteral("functionBtn_small"));
    connect(linkToggleBtn, &QPushButton::clicked, this, &NetworkPage::toggleInterfaceLink);

    QHBoxLayout *toolbarRow = new QHBoxLayout(toolbar);
    toolbarRow->setContentsMargins(16, 8, 16, 8);
    toolbarRow->setSpacing(8);
    toolbarRow->setAlignment(Qt::AlignVCenter);
    toolbarRow->addWidget(
        TbWidget::createFormLabel(toolbar, tr("选择网卡"), TbWidget::styledControlHeight(ifaceBox)));
    toolbarRow->addWidget(ifaceBox, 1);
    toolbarRow->addWidget(refreshBtn, 0, Qt::AlignVCenter);
    toolbarRow->addWidget(linkToggleBtn, 0, Qt::AlignVCenter);
    toolbar->setLayout(toolbarRow);

    tabWidget = new QTabWidget(content);
    tabWidget->setObjectName(QStringLiteral("networkTabs"));
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *statusTab = new QWidget(tabWidget);

    QGroupBox *ifaceInfoGroup = new QGroupBox(tr("接口状态"), statusTab);
    statusMacLabel = new QLabel(ifaceInfoGroup);
    statusIpLabel = new QLabel(ifaceInfoGroup);
    statusGwLabel = new QLabel(ifaceInfoGroup);
    statusDnsLabel = new QLabel(ifaceInfoGroup);
    statusLinkLabel = new QLabel(ifaceInfoGroup);
    statusBackendLabel = new QLabel(ifaceInfoGroup);
    const QList<QLabel *> statusLabels = {statusMacLabel, statusIpLabel, statusGwLabel,
                                          statusDnsLabel, statusLinkLabel, statusBackendLabel};
    for (QLabel *label : statusLabels) {
        label->setWordWrap(true);
    }

    QVBoxLayout *statusLeft = new QVBoxLayout;
    statusLeft->setSpacing(6);
    statusLeft->addWidget(statusMacLabel);
    statusLeft->addWidget(statusIpLabel);
    statusLeft->addWidget(statusGwLabel);
    QVBoxLayout *statusRight = new QVBoxLayout;
    statusRight->setSpacing(6);
    statusRight->addWidget(statusDnsLabel);
    statusRight->addWidget(statusLinkLabel);
    statusRight->addWidget(statusBackendLabel);
    QHBoxLayout *ifaceInfoCols = new QHBoxLayout(ifaceInfoGroup);
    ifaceInfoCols->setContentsMargins(12, 16, 12, 12);
    ifaceInfoCols->setSpacing(24);
    ifaceInfoCols->addLayout(statusLeft, 1);
    ifaceInfoCols->addLayout(statusRight, 1);

    QGroupBox *pingGroup = new QGroupBox(tr("Ping 测试"), statusTab);
    pingHostEdit = new QLineEdit(pingGroup);
    pingHostEdit->setObjectName(QStringLiteral("inputLineEdit"));
    pingHostEdit->setText(QStringLiteral("8.8.8.8"));
    TbWidget::applyLineEditStyle(pingHostEdit);
    pingPlayBtn = new QPushButton(tr("Ping"), pingGroup);
    pingPlayBtn->setObjectName(QStringLiteral("functionBtn_small"));
    pingPlayBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QWidget *pingHostRowWidget = new QWidget(pingGroup);
    pingHostRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *pingHostRow = new QHBoxLayout(pingHostRowWidget);
    pingHostRow->setContentsMargins(0, 0, 0, 0);
    pingHostRow->setSpacing(8);
    pingHostRow->setAlignment(Qt::AlignVCenter);
    pingHostRow->addWidget(pingHostEdit, 1);
    pingHostRow->addWidget(pingPlayBtn, 0, Qt::AlignVCenter);

    pingCountSpin = new QSpinBox(pingGroup);
    pingCountSpin->setRange(1, 20);
    pingCountSpin->setValue(4);
    TbWidget::applySpinBoxStyle(pingCountSpin);
    pingInfiniteCheck = new QCheckBox(tr("无限次"), pingGroup);
    QWidget *pingCountRowWidget = new QWidget(pingGroup);
    pingCountRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *pingCountRow = new QHBoxLayout(pingCountRowWidget);
    pingCountRow->setContentsMargins(0, 0, 0, 0);
    pingCountRow->setSpacing(12);
    pingCountRow->setAlignment(Qt::AlignVCenter);
    pingCountRow->addWidget(pingCountSpin);
    pingCountRow->addWidget(pingInfiniteCheck);
    pingCountRow->addStretch();

    TbWidget::FormGridBuilder pingGrid(pingGroup);
    pingGrid.addLabeledRow(tr("目标"), pingHostRowWidget);
    pingGrid.addLabeledRow(tr("次数"), pingCountRowWidget);

    pingLogArea = new QTextEdit(statusTab);
    pingLogArea->setReadOnly(true);
    pingLogArea->setObjectName(QStringLiteral("networkLogArea"));
    pingLogArea->setMaximumHeight(150);
    pingLogArea->setMinimumHeight(96);

    QVBoxLayout *statusLayout = new QVBoxLayout(statusTab);
    statusLayout->setContentsMargins(12, 12, 12, 12);
    statusLayout->setSpacing(12);
    statusLayout->addWidget(ifaceInfoGroup, 0);
    statusLayout->addWidget(pingGroup, 0);
    statusLayout->addWidget(pingLogArea, 0);
    statusLayout->addStretch(1);
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
    QWidget *macBtnRowWidget = new QWidget(macGroup);
    macBtnRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *macBtnRow = new QHBoxLayout(macBtnRowWidget);
    macBtnRow->setContentsMargins(0, 0, 0, 0);
    macBtnRow->setSpacing(8);
    macBtnRow->setAlignment(Qt::AlignVCenter);
    macBtnRow->addWidget(macApplyBtn);
    macBtnRow->addWidget(macSaveBtn);
    macBtnRow->addStretch();
    TbWidget::FormGridBuilder macGrid(macGroup);
    macGrid.addLabeledRow(tr("新 MAC"), macEdit);
    macGrid.addFieldRow(macBtnRowWidget);

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
    QWidget *ipBtnRowWidget = new QWidget(ipGroup);
    ipBtnRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *ipBtnRow = new QHBoxLayout(ipBtnRowWidget);
    ipBtnRow->setContentsMargins(0, 0, 0, 0);
    ipBtnRow->setSpacing(8);
    ipBtnRow->setAlignment(Qt::AlignVCenter);
    ipBtnRow->addWidget(ipApplyBtn);
    ipBtnRow->addWidget(ipSaveBtn);
    ipBtnRow->addStretch();

    TbWidget::FormGridBuilder ipGrid(ipGroup);
    ipGrid.addFieldRow(dhcpRadio);
    ipGrid.addFieldRow(staticRadio);
    ipGrid.addLabeledRow(tr("IP 地址"), ipEdit);
    ipGrid.addLabeledRow(tr("子网掩码"), maskEdit);
    ipGrid.addLabeledRow(tr("前缀长度"), prefixSpin);
    ipGrid.addLabeledRow(tr("默认网关"), gatewayEdit);
    ipGrid.addLabeledRow(tr("DNS"), dnsEdit);
    ipGrid.addFieldRow(ipBtnRowWidget);

    ipv4LogArea = new QTextEdit(ipv4Tab);
    ipv4LogArea->setReadOnly(true);
    ipv4LogArea->setObjectName(QStringLiteral("networkLogArea"));
    QVBoxLayout *ipv4Layout = new QVBoxLayout(ipv4Tab);
    ipv4Layout->setContentsMargins(8, 8, 8, 8);
    ipv4Layout->addWidget(ipGroup);
    ipv4Layout->addWidget(ipv4LogArea, 1);
    tabWidget->addTab(ipv4Tab, tr("IPv4"));

    QWidget *i210Tab = new QWidget(tabWidget);
    buildI210Tab(i210Tab);
    i210TabIndex = tabWidget->addTab(i210Tab, tr("高级 I210"));
    if (!TbNet::hasI210Nic()) {
        tabWidget->setTabEnabled(i210TabIndex, false);
        tabWidget->setTabToolTip(i210TabIndex, tr("未检测到 Intel I210 网卡"));
    }

    QWidget *tabContainer = new QWidget(content);
    QHBoxLayout *tabContainerLayout = new QHBoxLayout(tabContainer);
    tabContainerLayout->setContentsMargins(16, 0, 16, 8);
    tabContainerLayout->setSpacing(0);
    tabContainerLayout->addWidget(tabWidget);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(toolbar);
    pageLayout->addWidget(tabContainer, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &NetworkPage::refreshInterfaces);
    connect(ifaceBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &NetworkPage::onInterfaceChanged);
    connect(macApplyBtn, &QPushButton::clicked, this, &NetworkPage::applyMacRuntime);
    connect(macSaveBtn, &QPushButton::clicked, this, &NetworkPage::saveMacPersistent);
    connect(ipApplyBtn, &QPushButton::clicked, this, &NetworkPage::applyIpv4Runtime);
    connect(ipSaveBtn, &QPushButton::clicked, this, &NetworkPage::saveIpv4Persistent);
    connect(pingPlayBtn, &QPushButton::clicked, this, &NetworkPage::onPingPlayBtnClicked);
    connect(pingInfiniteCheck, &QCheckBox::toggled, this, [this](bool infinite) {
        if (pingCountSpin) {
            pingCountSpin->setEnabled(!infinite);
        }
    });

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

    QWidget *i210BtnRowWidget = new QWidget(i210Group);
    i210BtnRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *i210BtnRow = new QHBoxLayout(i210BtnRowWidget);
    i210BtnRow->setContentsMargins(0, 0, 0, 0);
    i210BtnRow->setSpacing(8);
    i210BtnRow->setAlignment(Qt::AlignVCenter);
    i210BtnRow->addWidget(editBtn);
    i210BtnRow->addWidget(checkBtn);
    i210BtnRow->addWidget(dumpBtn);
    i210BtnRow->addWidget(writeBtn);
    i210BtnRow->addStretch();

    TbWidget::FormGridBuilder i210Grid(i210Group);
    i210Grid.addLabeledRow(tr("MAC"), i210MacLine);
    i210Grid.addFieldRow(i210BtnRowWidget);

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
    if (!TbNet::hasNetAdminPrivilege()) {
        return tr("提示：当前非 root，修改 MAC/IPv4 与接口 up/down 不可用；Ping 仍可用。");
    }
    return tr("已具备网络管理权限（root）。");
}

void NetworkPage::updatePingPlayButton()
{
    if (!pingPlayBtn) {
        return;
    }
    pingPlayBtn->setText(pingRunning ? tr("停止") : tr("Ping"));
}

void NetworkPage::updatePageStatusBar(const QString &pingHint)
{
    if (!pingHint.isEmpty()) {
        setStatusMessage(pingHint);
        return;
    }
    if (pingRunning) {
        const QString iface = currentInterfaceName();
        const QString host = pingHostEdit ? pingHostEdit->text().trimmed() : QString();
        if (pingInfiniteCheck && pingInfiniteCheck->isChecked()) {
            setStatusMessage(tr("正在经 %1 持续 Ping %2 …").arg(iface, host));
        } else {
            setStatusMessage(tr("正在经 %1 Ping %2 …").arg(iface, host));
        }
        return;
    }
    setStatusMessage(defaultStatusHint());
}

void NetworkPage::applyPrivilegeUi()
{
    updatePageStatusBar();
    const bool canAdmin = TbNet::hasNetAdminPrivilege();
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
    interfaces = TbNet::listInterfaces();
    ifaceBox->blockSignals(true);
    ifaceBox->clear();
    int restoreIndex = 0;
    for (int i = 0; i < interfaces.size(); ++i) {
        const TbNet::InterfaceInfo &info = interfaces.at(i);
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
        appendMacLog(tr("未找到可用物理网卡。"));
        updateStatusLabels();
        updatePageStatusBar();
    }
    syncLinkToggleButton();
}

void NetworkPage::onInterfaceChanged(int index)
{
    if (index < 0 || index >= interfaces.size()) {
        return;
    }
    const TbNet::InterfaceInfo info = TbNet::readInterfaceConfig(interfaces.at(index).name);
    interfaces[index] = info;
    updateStatusLabels();
    updatePageStatusBar();
    fillConfigFields(info);
}

void NetworkPage::updateStatusLabels()
{
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        const QString dash = tr("—");
        if (statusMacLabel) {
            setInfoLine(statusMacLabel, tr("MAC"), dash);
            setInfoLine(statusIpLabel, tr("IPv4"), dash);
            setInfoLine(statusGwLabel, tr("网关"), dash);
            setInfoLine(statusDnsLabel, tr("DNS"), dash);
            setInfoLine(statusLinkLabel, tr("链路"), dash);
            setInfoLine(statusBackendLabel, tr("配置后端"), dash);
        }
        return;
    }
    const TbNet::InterfaceInfo info = TbNet::readInterfaceConfig(iface);
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
                TbNet::backendName(TbNet::detectNetworkBackend()));
    syncLinkToggleButton();
}

void NetworkPage::fillConfigFields(const TbNet::InterfaceInfo &info)
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
    if (TbNet::hasNetAdminPrivilege()) {
        return true;
    }
    QMessageBox::warning(this, tr("权限不足"),
                         tr("修改网络配置需要 root 权限，请使用 sudo 运行 TuxiBit。"));
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
    const bool ok = TbNet::applyRuntimeMac(iface, mac, &log);
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
    TbNet::persistMacAddress(iface, mac, &log);
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
        ok = TbNet::applyRuntimeDhcp(iface, &log);
    } else {
        const int prefix = prefixFromMaskField(maskEdit->text().trimmed(), prefixSpin->value());
        const QStringList dns =
            dnsEdit->text().split(QRegExp(QStringLiteral("[,\\s]+")), QString::SkipEmptyParts);
        if (!confirmNetworkChange(
                tr("应用静态 IP"),
                tr("将为 %1 设置临时静态地址 %2/%3\n是否继续？").arg(iface, ipEdit->text()).arg(prefix))) {
            return;
        }
        ok = TbNet::applyRuntimeIpv4Static(iface, ipEdit->text().trimmed(), prefix,
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
    TbNet::persistIpv4Config(iface, dhcpRadio->isChecked(), ipEdit->text().trimmed(), prefix,
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
    TbNet::setLinkState(currentInterfaceName(), true, &log);
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
    TbNet::setLinkState(currentInterfaceName(), false, &log);
    appendMacLog(log);
    refreshInterfaces();
    syncLinkToggleButton();
}

void NetworkPage::onPingPlayBtnClicked()
{
    if (pingRunning) {
        stopPing();
    } else {
        startPing();
    }
}

void NetworkPage::startPing()
{
    const QString iface = currentInterfaceName();
    if (iface.isEmpty()) {
        QMessageBox::information(this, tr("Ping"), tr("请先选择网卡。"));
        return;
    }
    const QString host = pingHostEdit->text().trimmed();
    if (host.isEmpty()) {
        QMessageBox::information(this, tr("Ping"), tr("请输入目标地址。"));
        return;
    }
    const int count = (pingInfiniteCheck && pingInfiniteCheck->isChecked())
        ? 0
        : pingCountSpin->value();
    pingRunning = true;
    updatePingPlayButton();
    updatePageStatusBar();
    appendPingLog(tr("--- 经 %1 Ping %2%3 ---")
                      .arg(iface, host, count > 0 ? QString() : tr("（无限次）")));
    pingRunner->start(host, count, iface);
}

void NetworkPage::stopPing()
{
    pingRunner->stop();
    pingRunning = false;
    updatePingPlayButton();
    updatePageStatusBar();
}

void NetworkPage::onPingLine(const QString &line)
{
    appendPingLog(line);
    if (line.contains(QStringLiteral("packet loss")) || line.contains(QStringLiteral("丢包"))) {
        updatePageStatusBar(line);
    }
}

void NetworkPage::onPingFinished(int exitCode)
{
    pingRunning = false;
    updatePingPlayButton();
    appendPingLog(tr("Ping 结束，退出码 %1").arg(exitCode));
    updatePageStatusBar(tr("Ping 结束，退出码 %1").arg(exitCode));
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
    const TbNet::CommandResult result =
        TbNet::runCommand(TbPaths::eepromArmTool(), QStringList());
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
    const TbNet::CommandResult result = TbNet::runCommand(
        TbPaths::eepromArmTool(), QStringList() << QStringLiteral("-dump") << QStringLiteral("-NIC=1"));
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

    QFile i210File(TbPaths::i210OriginOtp());
    if (!i210File.exists()) {
        i210LogArea->setText(tr("错误：找不到 I210NIC-origin.otp（%1）").arg(i210File.fileName()));
        return false;
    }
    if (!i210File.open(QIODevice::ReadOnly)) {
        i210LogArea->setText(tr("错误：无法打开 %1").arg(i210File.fileName()));
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
    const QString cmd = QStringLiteral("%1 -write -NIC=1 -f=%2")
                          .arg(TbPaths::eepromArmTool(), i210TmpFile.fileName());
    const TbNet::CommandResult result =
        TbNet::runCommand(QStringLiteral("/bin/sh"), QStringList() << QStringLiteral("-c") << cmd);
    i210LogArea->clear();
    for (const QString &line : result.standardOutput.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        i210LogArea->append(line);
    }
    if (result.standardOutput.contains(QStringLiteral("Flash update complete"))) {
        QMessageBox msg(this);
        msg.setText(tr("烧录完成。是否立即重启？"));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msg.exec() == QMessageBox::Yes) {
            TbNet::runShell(QStringLiteral("reboot"));
        }
    }
}

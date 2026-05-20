#include "upgradepage.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

void setInfoLine(QLabel *label, const QString &name, const QString &value)
{
    label->setText(QStringLiteral("%1：%2").arg(name, value));
}

QString formatFileSize(qint64 bytes)
{
    if (bytes < 1024) {
        return QStringLiteral("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    }
    return QStringLiteral("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
}

} // namespace

UpgradePage::UpgradePage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("系统升级"));
    updateRunner = new EbUpdate::UpdateRunner(this);
    connect(updateRunner, &EbUpdate::UpdateRunner::lineOutput, this, &UpgradePage::onUpdateLine);
    connect(updateRunner, &EbUpdate::UpdateRunner::finished, this, &UpgradePage::onUpdateFinished);

    buildUi();

    const QString hint = EbUpdate::findRemovablePackageHint();
    if (!hint.isEmpty()) {
        packagePathEdit->setText(hint);
    }
}

void UpgradePage::buildUi()
{
    QWidget *content = contentArea();

    selectBtn = new QPushButton(tr("选择包"), content);
    selectBtn->setObjectName(QStringLiteral("functionBtn_small"));
    verifyBtn = new QPushButton(tr("校验"), content);
    verifyBtn->setObjectName(QStringLiteral("functionBtn_small"));
    startBtn = new QPushButton(tr("开始升级"), content);
    startBtn->setObjectName(QStringLiteral("functionBtn_small"));
    shutdownBtn = new QPushButton(tr("关机"), content);
    shutdownBtn->setObjectName(QStringLiteral("functionBtn_small"));
    rebootBtn = new QPushButton(tr("重启"), content);
    rebootBtn->setObjectName(QStringLiteral("functionBtn_small"));
    connect(selectBtn, &QPushButton::clicked, this, &UpgradePage::selectPackage);
    connect(verifyBtn, &QPushButton::clicked, this, &UpgradePage::verifyPackage);
    connect(startBtn, &QPushButton::clicked, this, &UpgradePage::startUpgrade);
    connect(shutdownBtn, &QPushButton::clicked, this, &UpgradePage::shutdownSystem);
    connect(rebootBtn, &QPushButton::clicked, this, &UpgradePage::rebootSystem);

    QHBoxLayout *actionRow = new QHBoxLayout;
    actionRow->addWidget(selectBtn);
    actionRow->addWidget(verifyBtn);
    actionRow->addWidget(startBtn);
    actionRow->addWidget(shutdownBtn);
    actionRow->addWidget(rebootBtn);
    actionRow->addStretch();

    QGroupBox *pkgGroup = new QGroupBox(tr("升级包"), content);
    packagePathEdit = new QLineEdit(pkgGroup);
    packagePathEdit->setObjectName(QStringLiteral("inputLineEdit"));
    packagePathEdit->setReadOnly(true);
    packagePathEdit->setPlaceholderText(tr("请选择 *.tar.gz 升级包"));

    QFormLayout *pkgForm = new QFormLayout(pkgGroup);
    pkgForm->setContentsMargins(12, 16, 12, 12);
    pkgForm->addRow(tr("路径"), packagePathEdit);

    QGroupBox *infoGroup = new QGroupBox(tr("包信息"), content);
    infoPathLabel = new QLabel(infoGroup);
    infoSizeLabel = new QLabel(infoGroup);
    infoVersionLabel = new QLabel(infoGroup);
    infoProductLabel = new QLabel(infoGroup);
    infoDescLabel = new QLabel(infoGroup);
    infoStatusLabel = new QLabel(infoGroup);
    infoDescLabel->setWordWrap(true);

    QVBoxLayout *infoLeft = new QVBoxLayout;
    infoLeft->setSpacing(4);
    infoLeft->addWidget(infoPathLabel);
    infoLeft->addWidget(infoSizeLabel);
    infoLeft->addWidget(infoVersionLabel);
    QVBoxLayout *infoRight = new QVBoxLayout;
    infoRight->setSpacing(4);
    infoRight->addWidget(infoProductLabel);
    infoRight->addWidget(infoDescLabel);
    infoRight->addWidget(infoStatusLabel);
    QHBoxLayout *infoCols = new QHBoxLayout;
    infoCols->addLayout(infoLeft, 1);
    infoCols->addLayout(infoRight, 1);

    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    infoLayout->setContentsMargins(12, 16, 12, 12);
    infoLayout->addLayout(infoCols);

    logArea = new QTextEdit(content);
    logArea->setReadOnly(true);
    logArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(10);
    pageLayout->addLayout(actionRow);
    pageLayout->addWidget(pkgGroup);
    pageLayout->addWidget(infoGroup);
    pageLayout->addWidget(logArea, 1);

    setInfoLine(infoPathLabel, tr("文件"), tr("—"));
    setInfoLine(infoSizeLabel, tr("大小"), tr("—"));
    setInfoLine(infoVersionLabel, tr("版本"), tr("—"));
    setInfoLine(infoProductLabel, tr("产品"), tr("—"));
    setInfoLine(infoDescLabel, tr("说明"), tr("—"));
    setInfoLine(infoStatusLabel, tr("状态"), tr("未校验"));

    syncActionButtons();
    setStatusMessage(defaultStatusHint());
}

QString UpgradePage::defaultStatusHint() const
{
    if (!EbUpdate::hasRootPrivilege()) {
        return tr("警告：升级将覆盖根目录；当前非 root，请使用 sudo 运行。");
    }
    return tr("警告：升级包将解压到 /，请确认备份；格式 *.tar.gz，可选 easybench-update.json。");
}

void UpgradePage::syncActionButtons()
{
    const bool busy = upgradeBusy;
    if (selectBtn) {
        selectBtn->setEnabled(!busy);
    }
    if (verifyBtn) {
        verifyBtn->setEnabled(!busy);
    }
    if (startBtn) {
        startBtn->setEnabled(!busy && canStartUpgrade());
    }
}

bool UpgradePage::canStartUpgrade() const
{
    return EbUpdate::hasRootPrivilege() && !upgradeBusy;
}

void UpgradePage::setUpgradeBusy(bool busy)
{
    if (upgradeBusy == busy) {
        return;
    }
    upgradeBusy = busy;
    syncActionButtons();
}

void UpgradePage::shutdownSystem()
{
    system("shutdown -h now");
}

void UpgradePage::rebootSystem()
{
    system("reboot");
}

void UpgradePage::appendLog(const QString &text)
{
    logArea->append(text);
}

void UpgradePage::refreshInfoLabels(const EbUpdate::PackageInfo &info)
{
    setInfoLine(infoPathLabel, tr("文件"), info.path.isEmpty() ? tr("—") : info.path);
    setInfoLine(infoSizeLabel, tr("大小"),
                info.fileSizeBytes > 0 ? formatFileSize(info.fileSizeBytes) : tr("—"));
    setInfoLine(infoVersionLabel, tr("版本"),
                info.hasManifest && !info.version.isEmpty() ? info.version : tr("—"));
    setInfoLine(infoProductLabel, tr("产品"),
                info.hasManifest && !info.product.isEmpty() ? info.product : tr("—"));
    setInfoLine(infoDescLabel, tr("说明"),
                info.hasManifest && !info.description.isEmpty() ? info.description : tr("—"));

    QString status;
    if (!info.errorMessage.isEmpty()) {
        status = info.errorMessage;
    } else if (info.archiveValid) {
        status = info.hasManifest ? tr("压缩包有效，已读取清单") : tr("压缩包有效（无清单）");
        if (!info.warningMessage.isEmpty()) {
            status += QStringLiteral("；") + info.warningMessage;
        }
    } else {
        status = tr("未校验");
    }
    setInfoLine(infoStatusLabel, tr("状态"), status);
}

void UpgradePage::selectPackage()
{
    QString startDir = QStringLiteral("/run/media");
    if (!QDir(startDir).exists()) {
        startDir = QDir::homePath();
    }
    const QString path = QFileDialog::getOpenFileName(
        this, tr("选择升级包"), startDir, tr("升级包 (*.tar.gz);;所有文件 (*)"));
    if (path.isEmpty()) {
        return;
    }
    packagePathEdit->setText(path);
    lastInfo = EbUpdate::PackageInfo();
    setInfoLine(infoStatusLabel, tr("状态"), tr("已选择，请校验"));
}

void UpgradePage::verifyPackage()
{
    const QString path = packagePathEdit->text().trimmed();
    if (path.isEmpty()) {
        QMessageBox::information(this, tr("校验"), tr("请先选择升级包。"));
        return;
    }

    lastInfo = EbUpdate::inspectPackage(path, g_opt->getProductInfo());
    refreshInfoLabels(lastInfo);
    appendLog(tr("--- 校验 %1 ---").arg(path));
    if (!lastInfo.errorMessage.isEmpty()) {
        appendLog(lastInfo.errorMessage);
    } else {
        appendLog(tr("压缩包校验通过。"));
    }
    if (!lastInfo.warningMessage.isEmpty()) {
        appendLog(lastInfo.warningMessage);
    }
}

bool UpgradePage::confirmUpgrade()
{
    QString detail = tr("将把升级包解压到根目录 /，可能覆盖系统文件并导致断网。\n\n包：%1")
                         .arg(packagePathEdit->text().trimmed());
    if (lastInfo.hasManifest && !lastInfo.version.isEmpty()) {
        detail += tr("\n版本：%1").arg(lastInfo.version);
    }
    return QMessageBox::question(this, tr("确认升级"), detail, QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No)
           == QMessageBox::Yes;
}

void UpgradePage::startUpgrade()
{
    if (!EbUpdate::hasRootPrivilege()) {
        QMessageBox::warning(this, tr("权限不足"),
                             tr("升级需要 root 权限，请使用 sudo 运行 EasyBench。"));
        return;
    }

    const QString path = packagePathEdit->text().trimmed();
    if (path.isEmpty()) {
        QMessageBox::information(this, tr("升级"), tr("请先选择升级包。"));
        return;
    }

    if (!lastInfo.archiveValid || lastInfo.path != path) {
        verifyPackage();
        if (!lastInfo.archiveValid || !lastInfo.errorMessage.isEmpty()) {
            QMessageBox::warning(this, tr("升级"), tr("请先通过校验。"));
            return;
        }
    }

    if (!confirmUpgrade()) {
        return;
    }

    setUpgradeBusy(true);
    setStatusMessage(tr("正在升级，请勿关闭程序…"));
    appendLog(tr("--- 开始升级 ---"));
    updateRunner->start(path);
}

void UpgradePage::onUpdateLine(const QString &line)
{
    appendLog(line);
}

void UpgradePage::onUpdateFinished(bool success, int exitCode)
{
    Q_UNUSED(exitCode);
    setUpgradeBusy(false);
    setStatusMessage(success ? tr("升级完成。") : tr("升级失败，详见日志。"));

    if (!success) {
        QMessageBox::warning(this, tr("升级"), tr("升级失败，详见日志。"));
        return;
    }

    appendLog(tr("升级完成。"));
    QMessageBox::information(this, tr("升级"), tr("升级成功。"));

    if (lastInfo.rebootAfter) {
        if (QMessageBox::question(this, tr("重启"), tr("清单要求升级后重启，是否立即重启？"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
            == QMessageBox::Yes) {
            EbUpdate::runShell(QStringLiteral("reboot"));
        }
    }
}

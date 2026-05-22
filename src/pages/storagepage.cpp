#include "storagepage.h"
#include "widgets/tb_widget_util.h"
#include "tb_thread_util.h"

#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegExp>
#include <QVBoxLayout>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

double parseDdMbps(const QString &line)
{
    const QRegExp rx(QStringLiteral("([0-9.]+)\\s*MB/s"));
    if (rx.indexIn(line) >= 0) {
        return rx.cap(1).toDouble();
    }
    return -1.0;
}

QString formatDurationMs(qint64 ms)
{
    if (ms < 1000) {
        return QStringLiteral("%1 ms").arg(ms);
    }
    return QStringLiteral("%1 s").arg(ms / 1000.0, 0, 'f', 2);
}

QString testKindLabel(StorageTestWorker::TestKind kind)
{
    switch (kind) {
    case StorageTestWorker::TestKind::MemoryStress:
        return QStringLiteral("内存压力测试");
    case StorageTestWorker::TestKind::DiskRead:
        return QStringLiteral("块设备顺序读");
    case StorageTestWorker::TestKind::DiskWrite:
        return QStringLiteral("块设备顺序写");
    }
    return QStringLiteral("未知");
}

} // namespace

StorageTestWorker::StorageTestWorker(TestKind kind,
                                     const QString &targetPath,
                                     int sizeMb,
                                     int loops,
                                     QObject *parent)
    : QThread(parent)
    , m_kind(kind)
    , m_targetPath(targetPath)
    , m_sizeMb(sizeMb)
    , m_loops(loops)
{
}

void StorageTestWorker::run()
{
    TbThread::setCurrentThreadName("eb-storage");
    m_timer.start();
    m_lastDdMbps = -1.0;

    bool ok = false;
    QString summary;
    switch (m_kind) {
    case TestKind::MemoryStress:
        ok = runMemoryTest();
        summary = ok ? tr("内存压力测试通过") : tr("内存压力测试失败");
        break;
    case TestKind::DiskRead:
        ok = runDiskRead();
        summary = ok ? tr("磁盘读取测试完成") : tr("磁盘读取测试失败");
        break;
    case TestKind::DiskWrite:
        ok = runDiskWrite();
        summary = ok ? tr("磁盘写入测试完成") : tr("磁盘写入测试失败");
        break;
    }
    emitTestReport(ok);
    emit finished(ok, summary);
}

void StorageTestWorker::emitTestReport(bool ok)
{
    const qint64 elapsedMs = m_timer.elapsed();
    const qint64 totalMb = static_cast<qint64>(m_sizeMb) * m_loops;
    const qint64 totalBytes = totalMb * 1024LL * 1024LL;

    emit logLine(QString());
    emit logLine(tr("======== 测试报告 ========"));
    emit logLine(tr("测试类型：%1").arg(testKindLabel(m_kind)));
    emit logLine(tr("测试目标：%1").arg(m_targetPath));
    emit logLine(tr("数据量：%1 MB × %2 轮 = %3 MB")
                     .arg(m_sizeMb)
                     .arg(m_loops)
                     .arg(totalMb));
    emit logLine(tr("耗时：%1").arg(formatDurationMs(elapsedMs)));

    if (m_kind == TestKind::MemoryStress) {
        const double sec = elapsedMs / 1000.0;
        if (sec > 0.0) {
            const double verifyMbps = totalBytes / (1024.0 * 1024.0) / sec;
            emit logLine(tr("校验吞吐：约 %1 MB/s").arg(verifyMbps, 0, 'f', 1));
        }
    } else if (m_lastDdMbps > 0.0) {
        emit logLine(tr("dd 报告速率：%1 MB/s").arg(m_lastDdMbps, 0, 'f', 1));
    } else if (elapsedMs > 0) {
        const double avgMbps = totalBytes / (1024.0 * 1024.0) / (elapsedMs / 1000.0);
        emit logLine(tr("平均速率：约 %1 MB/s").arg(avgMbps, 0, 'f', 1));
    }

    emit logLine(tr("结果：%1").arg(ok ? tr("通过") : tr("失败")));
    emit logLine(tr("========================"));
}

bool StorageTestWorker::runMemoryTest()
{
    const qint64 bytes = static_cast<qint64>(m_sizeMb) * 1024 * 1024;
    if (bytes <= 0) {
        emit logLine(tr("无效测试大小"));
        return false;
    }

    char *buffer = static_cast<char *>(malloc(static_cast<size_t>(bytes)));
    if (!buffer) {
        emit logLine(tr("无法分配 %1 MB 内存").arg(m_sizeMb));
        return false;
    }

    emit logLine(tr("已分配 %1 MB，开始 memtester 风格校验…").arg(m_sizeMb));

    const unsigned char patterns[] = {0x00, 0xFF, 0xAA, 0x55, 0x5A, 0xA5};
    const int patternCount = static_cast<int>(sizeof(patterns) / sizeof(patterns[0]));

    for (int loop = 0; loop < m_loops && !isInterruptionRequested(); ++loop) {
        emit logLine(tr("---- 轮次 %1 / %2 ----").arg(loop + 1).arg(m_loops));

        for (int p = 0; p < patternCount && !isInterruptionRequested(); ++p) {
            const unsigned char pat = patterns[p];
            memset(buffer, pat, static_cast<size_t>(bytes));
            for (qint64 i = 0; i < bytes; ++i) {
                if (static_cast<unsigned char>(buffer[i]) != pat) {
                    emit logLine(tr("模式 0x%1 校验失败，偏移 %2")
                                     .arg(pat, 2, 16, QLatin1Char('0'))
                                     .arg(i));
                    free(buffer);
                    return false;
                }
            }
            emit progressChanged((loop * patternCount + p + 1) * 100 / (m_loops * patternCount));
        }

        /* 逐位行走测试 */
        for (int bit = 0; bit < 8 && !isInterruptionRequested(); ++bit) {
            const unsigned char mask = static_cast<unsigned char>(1U << bit);
            memset(buffer, mask, static_cast<size_t>(bytes));
            for (qint64 i = 0; i < bytes; ++i) {
                if (static_cast<unsigned char>(buffer[i]) != mask) {
                    emit logLine(tr("行走位测试失败，位 %1 偏移 %2").arg(bit).arg(i));
                    free(buffer);
                    return false;
                }
            }
        }
    }

    free(buffer);
    emit progressChanged(100);
    emit logLine(tr("内存测试全部通过"));
    return !isInterruptionRequested();
}

bool StorageTestWorker::runDiskRead()
{
    if (m_targetPath.isEmpty()) {
        emit logLine(tr("未指定块设备"));
        return false;
    }
    for (int loop = 0; loop < m_loops && !isInterruptionRequested(); ++loop) {
        emit logLine(tr("---- 读取轮次 %1 / %2 ----").arg(loop + 1).arg(m_loops));
        const QString cmd = QStringLiteral("dd if=%1 of=/dev/null bs=1M count=%2 2>&1")
                                .arg(m_targetPath)
                                .arg(m_sizeMb);
        emit logLine(tr("执行：%1").arg(cmd));
        FILE *fp = popen(cmd.toLocal8Bit().constData(), "r");
        if (!fp) {
            emit logLine(tr("无法启动 dd"));
            return false;
        }
        char buf[512];
        while (fgets(buf, sizeof(buf), fp) != nullptr) {
            const QString line = QString::fromLocal8Bit(buf).trimmed();
            emit logLine(line);
            const double mbps = parseDdMbps(line);
            if (mbps > 0.0) {
                m_lastDdMbps = mbps;
            }
            if (isInterruptionRequested()) {
                pclose(fp);
                return false;
            }
        }
        if (pclose(fp) != 0) {
            return false;
        }
        emit progressChanged((loop + 1) * 100 / m_loops);
    }
    return !isInterruptionRequested();
}

bool StorageTestWorker::runDiskWrite()
{
    if (m_targetPath.isEmpty()) {
        emit logLine(tr("未指定块设备"));
        return false;
    }
    for (int loop = 0; loop < m_loops && !isInterruptionRequested(); ++loop) {
        emit logLine(tr("---- 写入轮次 %1 / %2 ----").arg(loop + 1).arg(m_loops));
        const QString cmd = QStringLiteral("dd if=/dev/zero of=%1 bs=1M count=%2 2>&1")
                                .arg(m_targetPath)
                                .arg(m_sizeMb);
        emit logLine(tr("执行：%1").arg(cmd));
        FILE *fp = popen(cmd.toLocal8Bit().constData(), "r");
        if (!fp) {
            emit logLine(tr("无法启动 dd"));
            return false;
        }
        char buf[512];
        while (fgets(buf, sizeof(buf), fp) != nullptr) {
            const QString line = QString::fromLocal8Bit(buf).trimmed();
            emit logLine(line);
            const double mbps = parseDdMbps(line);
            if (mbps > 0.0) {
                m_lastDdMbps = mbps;
            }
            if (isInterruptionRequested()) {
                pclose(fp);
                return false;
            }
        }
        if (pclose(fp) != 0) {
            return false;
        }
        emit progressChanged((loop + 1) * 100 / m_loops);
    }
    return !isInterruptionRequested();
}

StoragePage::StoragePage(TbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("存储测试"));
    buildUi();
    refreshTargets();

}

StoragePage::~StoragePage()
{
    stopTest();
}

void StoragePage::buildUi()
{
    QWidget *content = contentArea();
    QGroupBox *cfgGroup = new QGroupBox(tr("测试配置"), content);

    mediaBox = new QComboBox(cfgGroup);
    mediaBox->setMinimumWidth(260);
    testTypeBox = new QComboBox(cfgGroup);
    testTypeBox->addItem(tr("内存压力 (memtester)"),
                         static_cast<int>(StorageTestWorker::TestKind::MemoryStress));
    testTypeBox->addItem(tr("块设备顺序读"), static_cast<int>(StorageTestWorker::TestKind::DiskRead));
    testTypeBox->addItem(tr("块设备顺序写 (破坏性)"),
                         static_cast<int>(StorageTestWorker::TestKind::DiskWrite));

    sizeSpin = new QSpinBox(cfgGroup);
    sizeSpin->setRange(1, 4096);
    sizeSpin->setValue(64);
    sizeSpin->setSuffix(tr(" MB"));

    loopsSpin = new QSpinBox(cfgGroup);
    loopsSpin->setRange(1, 100);
    loopsSpin->setValue(1);

    QPushButton *refreshBtn = new QPushButton(tr("刷新"), cfgGroup);
    refreshBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QWidget *mediaRowWidget = new QWidget(cfgGroup);
    mediaRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *mediaRow = new QHBoxLayout(mediaRowWidget);
    mediaRow->setContentsMargins(0, 0, 0, 0);
    mediaRow->setSpacing(8);
    mediaRow->setAlignment(Qt::AlignVCenter);
    mediaRow->addWidget(mediaBox, 1);
    mediaRow->addWidget(refreshBtn, 0, Qt::AlignVCenter);

    startBtn = new QPushButton(tr("开始测试"), cfgGroup);
    startBtn->setObjectName(QStringLiteral("functionBtn_small"));
    stopBtn = new QPushButton(tr("停止"), cfgGroup);
    stopBtn->setObjectName(QStringLiteral("functionBtn_small"));
    stopBtn->setEnabled(false);

    QWidget *btnRowWidget = new QWidget(cfgGroup);
    btnRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *btnRow = new QHBoxLayout(btnRowWidget);
    btnRow->setContentsMargins(0, 0, 0, 0);
    btnRow->setSpacing(8);
    btnRow->setAlignment(Qt::AlignVCenter);
    btnRow->addWidget(startBtn);
    btnRow->addWidget(stopBtn);
    btnRow->addStretch();

    QFormLayout *form = new QFormLayout;
    form->setContentsMargins(12, 16, 12, 12);
    form->addRow(tr("测试媒介"), mediaRowWidget);
    form->addRow(tr("测试类型"), testTypeBox);
    form->addRow(tr("数据量"), sizeSpin);
    form->addRow(tr("循环次数"), loopsSpin);
    form->addRow(QString(), btnRowWidget);
    TbWidget::applyFormLayoutStyle(form);
    cfgGroup->setLayout(form);

    QGroupBox *logGroup = new QGroupBox(tr("测试日志"), content);
    progressBar = new QProgressBar(logGroup);
    progressBar->setRange(0, 100);
    logArea = new QTextEdit(logGroup);
    logArea->setReadOnly(true);
    statusLabel = new QLabel(tr("就绪"), logGroup);
    statusLabel->setWordWrap(true);

    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(12, 16, 12, 12);
    logLayout->addWidget(progressBar);
    logLayout->addWidget(logArea, 1);
    logLayout->addWidget(statusLabel);

    QHBoxLayout *pageLayout = new QHBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->addWidget(cfgGroup, 0);
    pageLayout->addWidget(logGroup, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &StoragePage::refreshTargets);
    connect(startBtn, &QPushButton::clicked, this, &StoragePage::startTest);
    connect(stopBtn, &QPushButton::clicked, this, &StoragePage::stopTest);
    connect(testTypeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) {
                const auto kind = static_cast<StorageTestWorker::TestKind>(
                    testTypeBox->currentData().toInt());
                mediaBox->setEnabled(kind != StorageTestWorker::TestKind::MemoryStress);
            });
    testTypeBox->setCurrentIndex(0);
}

void StoragePage::refreshTargets()
{
    mediaBox->clear();
    mediaBox->addItem(tr("内存 (RAM)"), QStringLiteral(":memory:"));

    const QFileInfoList entries =
        QDir(QStringLiteral("/dev")).entryInfoList(QDir::System | QDir::Files);
    for (const QFileInfo &info : entries) {
        const QString name = info.fileName();
        if (name.startsWith(QStringLiteral("sd"))
            || name.startsWith(QStringLiteral("mmcblk"))
            || name.startsWith(QStringLiteral("nvme"))) {
            mediaBox->addItem(QStringLiteral("/dev/") + name,
                              QStringLiteral("/dev/") + name);
        }
    }
}

void StoragePage::setUiBusy(bool busy)
{
    startBtn->setEnabled(!busy);
    stopBtn->setEnabled(busy);
    mediaBox->setEnabled(!busy);
    testTypeBox->setEnabled(!busy);
    sizeSpin->setEnabled(!busy);
    loopsSpin->setEnabled(!busy);
}

void StoragePage::appendLog(const QString &line)
{
    logArea->append(line);
}

void StoragePage::startTest()
{
    if (worker && worker->isRunning()) {
        return;
    }

    const auto kind = static_cast<StorageTestWorker::TestKind>(testTypeBox->currentData().toInt());
    QString target;
    if (kind == StorageTestWorker::TestKind::MemoryStress) {
        target = QStringLiteral(":memory:");
    } else {
        target = mediaBox->currentData().toString();
        if (target.isEmpty() || target == QStringLiteral(":memory:")) {
            target = mediaBox->currentText();
        }
        if (kind == StorageTestWorker::TestKind::DiskWrite) {
            const auto answer = QMessageBox::warning(
                this, tr("存储测试"),
                tr("写入测试将覆盖 %1 上 %2 MB 数据，可能破坏分区内容。是否继续？")
                    .arg(target)
                    .arg(sizeSpin->value()),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return;
            }
        }
    }

    logArea->clear();
    progressBar->setValue(0);
    setUiBusy(true);
    statusLabel->setText(tr("测试进行中…"));

    worker = new StorageTestWorker(kind, target, sizeSpin->value(), loopsSpin->value(), this);
    TbThread::nameQThread(worker, "eb-storage");
    connect(worker, &StorageTestWorker::logLine, this, &StoragePage::onLogLine);
    connect(worker, &StorageTestWorker::progressChanged, this, &StoragePage::onProgress);
    connect(worker, &StorageTestWorker::finished, this, &StoragePage::onTestFinished);
    worker->start();
}

void StoragePage::stopTest()
{
    if (worker && worker->isRunning()) {
        worker->requestInterruption();
        statusLabel->setText(tr("正在停止…"));
    }
}

void StoragePage::onLogLine(const QString &line)
{
    appendLog(line);
}

void StoragePage::onProgress(int percent)
{
    progressBar->setValue(percent);
}

void StoragePage::onTestFinished(bool ok, const QString &summary)
{
    setUiBusy(false);
    statusLabel->setText(summary);
    appendLog(summary);
    if (worker) {
        worker->deleteLater();
        worker = nullptr;
    }
    if (!ok) {
        QMessageBox::warning(this, tr("存储测试"), summary);
    } else {
        QMessageBox::information(this, tr("存储测试"), summary);
    }
}

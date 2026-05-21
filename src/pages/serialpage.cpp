#include "serialpage.h"
#include "eb_thread_util.h"
#include "widgets/eb_widget_util.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QVBoxLayout>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <QtGlobal>
#include <unistd.h>

namespace {

QByteArray decodeHexPayload(const QString &text, bool *ok)
{
    QString compact = text;
    compact.remove(QRegularExpression(QStringLiteral("\\s")));
    if (compact.isEmpty()) {
        *ok = true;
        return QByteArray();
    }
    if (compact.size() % 2 != 0) {
        *ok = false;
        return QByteArray();
    }

    QByteArray out;
    out.reserve(compact.size() / 2);
    for (int i = 0; i < compact.size(); i += 2) {
        bool byteOk = false;
        const int value = compact.mid(i, 2).toInt(&byteOk, 16);
        if (!byteOk || value < 0 || value > 255) {
            *ok = false;
            return QByteArray();
        }
        out.append(static_cast<char>(value));
    }
    *ok = true;
    return out;
}

QString encodeHexDisplay(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QString();
    }
    QString out;
    out.reserve(data.size() * 3);
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0) {
            out.append(QLatin1Char(' '));
        }
        out.append(QStringLiteral("%1").arg(static_cast<unsigned char>(data.at(i)), 2, 16, QLatin1Char('0')));
    }
    return out.toUpper();
}

constexpr int kSerialReadChunk = 4096;
constexpr int kRecvFlushIntervalMs = 50;
constexpr qsizetype kRecvPendingMaxBytes = 64 * 1024;
constexpr int kRecvFlushMaxBytesPerTick = 4096;
constexpr int kRecvDisplayMaxChars = 48 * 1024;
constexpr int kRecvDisplayMaxBlocks = 400;

} // namespace

SerialRecvThread::SerialRecvThread(QObject *parent)
    : QThread(parent)
{
}

void SerialRecvThread::setSerialPortFd(int fd)
{
    serialFd = fd;
}

void SerialRecvThread::setDiscardIncoming(bool discard)
{
    discardIncoming.store(discard, std::memory_order_release);
}

void SerialRecvThread::run()
{
    EbThread::setCurrentThreadName("eb-serial");
    QByteArray batch;
    batch.reserve(kSerialReadChunk * 4);
    char buf[kSerialReadChunk];

    while (!isInterruptionRequested()) {
        if (serialFd < 0) {
            msleep(50);
            continue;
        }

        batch.clear();
        bool gotData = false;
        while (!isInterruptionRequested()) {
            const ssize_t ret = read(serialFd, buf, sizeof(buf));
            if (ret > 0) {
                batch.append(buf, static_cast<int>(ret));
                gotData = true;
                continue;
            }
            if (ret == 0) {
                break;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            return;
        }

        if (!batch.isEmpty() && !discardIncoming.load(std::memory_order_acquire)) {
            emit msgReceived(batch);
        } else if (!gotData) {
            msleep(5);
        }
    }
}

SerialPage::SerialPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("串口测试"));
    buildUi();
    refreshPortList();

    recvThread = new SerialRecvThread(this);
    EbThread::nameQThread(recvThread, "eb-serial");
    connect(recvThread, &SerialRecvThread::msgReceived, this, &SerialPage::appendRecvChunk);

    recvFlushTimer = new QTimer(this);
    recvFlushTimer->setInterval(kRecvFlushIntervalMs);
    connect(recvFlushTimer, &QTimer::timeout, this, &SerialPage::flushRecvDisplay);

    recvThread->start();
    setSerialStatus(defaultStatusHint());
}

SerialPage::~SerialPage()
{
    recvFlushTimer->stop();
    if (serialFd >= 0) {
        ::close(serialFd);
        serialFd = -1;
    }
    recvThread->setSerialPortFd(-1);
    if (recvThread->isRunning()) {
        recvThread->requestInterruption();
        recvThread->wait(2000);
    }
}

QString SerialPage::defaultStatusHint() const
{
    return tr("未打开");
}

void SerialPage::setSerialStatus(const QString &text)
{
    setStatusMessage(text);
}

void SerialPage::populateParamCombos()
{
    baudRateBox->clear();
    for (int i = 0; i < SerialParam::kBaudRateCount; ++i) {
        const int baud = SerialParam::kBaudRates[i];
        baudRateBox->addItem(QString::number(baud), baud);
    }
    baudRateBox->setCurrentIndex(0);

    dataBitsBox->clear();
    for (int i = 0; i < SerialParam::kDataBitsCount; ++i) {
        const int bits = SerialParam::kDataBits[i];
        dataBitsBox->addItem(QString::number(bits), bits);
    }
    dataBitsBox->setCurrentIndex(0);

    parityBox->clear();
    for (int i = 0; i < SerialParam::kParityCount; ++i) {
        const SerialParam::ParityOption &opt = SerialParam::kParityOptions[i];
        QString label;
        if (qstrcmp(opt.sttyValue, "none") == 0) {
            label = tr("无");
        } else if (qstrcmp(opt.sttyValue, "even") == 0) {
            label = tr("偶");
        } else {
            label = tr("奇");
        }
        parityBox->addItem(label, QString::fromLatin1(opt.sttyValue));
    }
    parityBox->setCurrentIndex(0);

    stopBitsBox->clear();
    for (int i = 0; i < SerialParam::kStopBitsCount; ++i) {
        const int bits = SerialParam::kStopBits[i];
        stopBitsBox->addItem(QString::number(bits), bits);
    }
    stopBitsBox->setCurrentIndex(0);
}

void SerialPage::buildUi()
{
    QWidget *content = contentArea();

    portGroup = new QGroupBox(tr("串口设置"), content);
    portGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    serialPortBox = new QComboBox(portGroup);
    refreshPortBtn = new QPushButton(tr("刷新"), portGroup);
    refreshPortBtn->setObjectName(QStringLiteral("functionBtn_small"));
    refreshPortBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    baudRateBox = new QComboBox(portGroup);

    dataBitsBox = new QComboBox(portGroup);
    parityBox = new QComboBox(portGroup);
    stopBitsBox = new QComboBox(portGroup);
    populateParamCombos();

    constexpr int kPortComboMinChars = 14;
    for (QComboBox *box :
         {serialPortBox, baudRateBox, dataBitsBox, parityBox, stopBitsBox}) {
        box->setMinimumContentsLength(kPortComboMinChars);
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    openBtn = new QPushButton(tr("打开"), portGroup);
    openBtn->setObjectName(QStringLiteral("functionBtn_small"));
    openBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget *portRowWidget = new QWidget(portGroup);
    portRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *portRow = new QHBoxLayout(portRowWidget);
    portRow->setContentsMargins(0, 0, 0, 0);
    portRow->setSpacing(8);
    portRow->setAlignment(Qt::AlignVCenter);
    portRow->addWidget(serialPortBox, 1);
    portRow->addWidget(refreshPortBtn, 0, Qt::AlignVCenter);

    QGridLayout *portGrid = new QGridLayout(portGroup);
    portGrid->setContentsMargins(12, 12, 12, 12);
    portGrid->setHorizontalSpacing(12);
    portGrid->setVerticalSpacing(6);
    portGrid->setColumnStretch(1, 1);

    auto addPortRow = [&](int row, const QString &labelText, QWidget *field) {
        field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        portGrid->addWidget(EbWidget::createFormLabel(portGroup, labelText), row, 0);
        portGrid->addWidget(field, row, 1);
    };

    addPortRow(0, tr("串口"), portRowWidget);
    addPortRow(1, tr("波特率"), baudRateBox);
    addPortRow(2, tr("数据位"), dataBitsBox);
    addPortRow(3, tr("校验"), parityBox);
    addPortRow(4, tr("停止位"), stopBitsBox);
    portGrid->addWidget(openBtn, 5, 1);

    echoGroup = new QGroupBox(tr("收发测试"), content);
    sendArea = new QTextEdit(echoGroup);
    sendArea->setPlainText(QStringLiteral("Hello from EasyBench"));
    sendArea->setMaximumHeight(100);
    recvArea = new QPlainTextEdit(echoGroup);
    recvArea->setReadOnly(true);
    recvArea->setMaximumBlockCount(kRecvDisplayMaxBlocks);
    recvArea->setLineWrapMode(QPlainTextEdit::NoWrap);
    sendHexCheck = new QCheckBox(tr("HEX 发送"), echoGroup);
    recvHexCheck = new QCheckBox(tr("HEX 显示"), echoGroup);
    recvPauseCheck = new QCheckBox(tr("暂停接收"), echoGroup);
    sendBtn = new QPushButton(tr("发送"), echoGroup);
    sendBtn->setObjectName(QStringLiteral("functionBtn_small"));
    sendClearBtn = new QPushButton(tr("清空发送"), echoGroup);
    sendClearBtn->setObjectName(QStringLiteral("functionBtn_small"));
    recvClearBtn = new QPushButton(tr("清空接收"), echoGroup);
    recvClearBtn->setObjectName(QStringLiteral("functionBtn_small"));

    QHBoxLayout *sendBtnRow = new QHBoxLayout;
    sendBtnRow->setAlignment(Qt::AlignVCenter);
    sendBtnRow->addWidget(sendHexCheck);
    sendBtnRow->addWidget(sendBtn);
    sendBtnRow->addWidget(sendClearBtn);
    sendBtnRow->addStretch();

    QHBoxLayout *recvBtnRow = new QHBoxLayout;
    recvBtnRow->setAlignment(Qt::AlignVCenter);
    recvBtnRow->addWidget(recvPauseCheck);
    recvBtnRow->addWidget(recvHexCheck);
    recvBtnRow->addWidget(recvClearBtn);
    recvBtnRow->addStretch();

    QVBoxLayout *echoLayout = new QVBoxLayout(echoGroup);
    echoLayout->setContentsMargins(12, 16, 12, 12);
    echoLayout->addWidget(new QLabel(tr("发送区"), echoGroup));
    echoLayout->addWidget(sendArea);
    echoLayout->addLayout(sendBtnRow);
    echoLayout->addWidget(new QLabel(tr("接收区"), echoGroup));
    echoLayout->addWidget(recvArea, 1);
    echoLayout->addLayout(recvBtnRow);

    QHBoxLayout *pageLayout = new QHBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(16);
    pageLayout->addWidget(portGroup, 0, Qt::AlignTop);
    pageLayout->addWidget(echoGroup, 1);

    echoGroup->setEnabled(false);

    connect(refreshPortBtn, &QPushButton::clicked, this, &SerialPage::refreshPortList);
    connect(serialPortBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPage::onPortChanged);
    connect(openBtn, &QPushButton::clicked, this, &SerialPage::openSerialPort);
    connect(sendBtn, &QPushButton::clicked, this, &SerialPage::sendSerialData);
    connect(sendClearBtn, &QPushButton::clicked, this, &SerialPage::clearSendArea);
    connect(recvClearBtn, &QPushButton::clicked, this, &SerialPage::clearRecvArea);
    connect(recvPauseCheck, &QCheckBox::toggled, this, &SerialPage::onRecvPauseToggled);
}

void SerialPage::refreshPortList()
{
    const int previous = serialPortBox->currentIndex();
    serialPortBox->blockSignals(true);
    serialPortBox->clear();

    QStringList ports;
    if (gSerialPortStr[0] != '\0') {
        ports.append(QString::fromUtf8(gSerialPortStr));
    }
    const QFileInfoList entries =
        QDir(QStringLiteral("/dev")).entryInfoList(QDir::System | QDir::Files);
    for (const QFileInfo &info : entries) {
        const QString name = info.fileName();
        if (name.startsWith(QStringLiteral("tty"))) {
            const QString path = QStringLiteral("/dev/") + name;
            if (!ports.contains(path)) {
                ports.append(path);
            }
        }
    }
    ports.sort();
    for (const QString &port : ports) {
        serialPortBox->addItem(port);
    }
    serialPortBox->blockSignals(false);

    if (serialPortBox->count() == 0) {
        setSerialStatus(tr("未检测到串口设备"));
        return;
    }
    if (previous >= 0 && previous < serialPortBox->count()) {
        serialPortBox->setCurrentIndex(previous);
    } else {
        serialPortBox->setCurrentIndex(0);
    }
    onPortChanged(serialPortBox->currentIndex());
}

void SerialPage::onPortChanged(int index)
{
    Q_UNUSED(index);
    const QString path = currentPortPath();
    if (!path.isEmpty()) {
        QByteArray bytes = path.toUtf8();
        strncpy(gSerialPortStr, bytes.constData(), sizeof(gSerialPortStr) - 1);
        gSerialPortStr[sizeof(gSerialPortStr) - 1] = '\0';
    }
    if (serialFd < 0) {
        setSerialStatus(tr("未打开 · %1").arg(path.isEmpty() ? tr("无设备") : path));
    }
}

QString SerialPage::currentPortPath() const
{
    return serialPortBox->currentText().trimmed();
}

int SerialPage::currentBaudRate() const
{
    bool ok = false;
    const int baud = baudRateBox->currentData().toInt(&ok);
    if (!ok || baud <= 0) {
        return SerialParam::kBaudRates[0];
    }
    return baud;
}

void SerialPage::applySerialPortStty()
{
    const QString path = currentPortPath();
    if (path.isEmpty() || !QFile::exists(path)) {
        return;
    }

    const int baud = currentBaudRate();
    const int dataBits = dataBitsBox->currentData().toInt();
    const QString parity = parityBox->currentData().toString();
    const int stopBits = stopBitsBox->currentData().toInt();

    QString csFlag = QStringLiteral("cs8");
    if (dataBits == 7) {
        csFlag = QStringLiteral("cs7");
    }

    QString parityFlag = QStringLiteral("-parenb");
    if (parity == QStringLiteral("even")) {
        parityFlag = QStringLiteral("parenb -parodd");
    } else if (parity == QStringLiteral("odd")) {
        parityFlag = QStringLiteral("parenb parodd");
    }

    QString stopFlag = QStringLiteral("-cstopb");
    if (stopBits == 2) {
        stopFlag = QStringLiteral("cstopb");
    }

    const QString cmd = QStringLiteral("stty -F %1 speed %2 %3 %4 %5 -echo")
                            .arg(path)
                            .arg(baud)
                            .arg(csFlag)
                            .arg(parityFlag)
                            .arg(stopFlag);
    if (system(cmd.toLocal8Bit().constData()) != 0) {
        setSerialStatus(tr("stty 配置失败：%1").arg(path));
    }
}

void SerialPage::applyParameters()
{
    applySerialPortStty();
}

QByteArray SerialPage::payloadFromSendArea(bool *hexOk) const
{
    const QString text = sendArea->toPlainText();
    if (!sendHexCheck->isChecked()) {
        if (hexOk) {
            *hexOk = true;
        }
        return text.toUtf8();
    }

    bool ok = false;
    const QByteArray payload = decodeHexPayload(text, &ok);
    if (hexOk) {
        *hexOk = ok;
    }
    return payload;
}

QString SerialPage::formatRecvData(const QByteArray &data) const
{
    if (recvHexCheck->isChecked()) {
        return encodeHexDisplay(data);
    }
    return QString::fromUtf8(data);
}

void SerialPage::openSerialPort()
{
    const QString path = currentPortPath();
    if (path.isEmpty()) {
        QMessageBox::warning(this, tr("串口"), tr("请选择串口设备。"));
        return;
    }

    if (openBtn->text() == tr("打开")) {
        applyParameters();
        serialFd = open(path.toUtf8().constData(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (serialFd < 0) {
            QMessageBox::warning(this, tr("串口"), tr("打开 %1 失败：%2").arg(path, strerror(errno)));
            return;
        }
        recvThread->setDiscardIncoming(false);
        recvPauseCheck->setChecked(false);
        recvThread->setSerialPortFd(serialFd);
        recvPending.clear();
        recvFlushTimer->start();
        echoGroup->setEnabled(true);
        openBtn->setText(tr("关闭"));
        setSerialStatus(tr("已打开 %1，波特率 %2").arg(path).arg(currentBaudRate()));
    } else {
        recvFlushTimer->stop();
        flushRecvDisplay();
        recvThread->setDiscardIncoming(false);
        recvPauseCheck->setChecked(false);
        if (serialFd >= 0) {
            ::close(serialFd);
            serialFd = -1;
        }
        recvThread->setSerialPortFd(-1);
        recvPending.clear();
        echoGroup->setEnabled(false);
        openBtn->setText(tr("打开"));
        setSerialStatus(tr("已关闭"));
    }
}

void SerialPage::onRecvPauseToggled(bool paused)
{
    recvThread->setDiscardIncoming(paused);
    if (paused) {
        recvPending.clear();
        recvFlushTimer->stop();
    } else if (serialFd >= 0) {
        recvFlushTimer->start();
    }
}

void SerialPage::appendRecvChunk(const QByteArray &data)
{
    if (data.isEmpty() || recvPauseCheck->isChecked()) {
        return;
    }
    recvPending.append(data);
    if (recvPending.size() > kRecvPendingMaxBytes) {
        recvPending.remove(0, recvPending.size() - static_cast<qsizetype>(kRecvPendingMaxBytes));
    }
}

bool SerialPage::recvAutoScroll() const
{
    QScrollBar *bar = recvArea->verticalScrollBar();
    return bar->value() + bar->pageStep() >= bar->maximum();
}

void SerialPage::trimRecvDocument()
{
    QTextDocument *doc = recvArea->document();
    if (doc->characterCount() <= kRecvDisplayMaxChars) {
        return;
    }
    const QString trimmed = recvArea->toPlainText().right(kRecvDisplayMaxChars);
    recvArea->setPlainText(trimmed);
}

void SerialPage::flushRecvDisplay()
{
    if (recvPending.isEmpty() || recvPauseCheck->isChecked()) {
        return;
    }

    const int takeBytes =
        qMin(static_cast<int>(recvPending.size()), kRecvFlushMaxBytesPerTick);
    const QByteArray chunk = recvPending.left(takeBytes);
    recvPending.remove(0, takeBytes);

    const bool scrollToEnd = recvAutoScroll();
    recvArea->setUpdatesEnabled(false);
    recvArea->appendPlainText(formatRecvData(chunk));
    trimRecvDocument();
    recvArea->setUpdatesEnabled(true);

    if (scrollToEnd) {
        recvArea->verticalScrollBar()->setValue(recvArea->verticalScrollBar()->maximum());
    }
}

void SerialPage::clearSendArea()
{
    sendArea->clear();
}

void SerialPage::clearRecvArea()
{
    recvPending.clear();
    recvArea->clear();
}

void SerialPage::sendSerialData()
{
    if (serialFd < 0) {
        QMessageBox::information(this, tr("串口"), tr("请先打开串口。"));
        return;
    }

    bool hexOk = true;
    const QByteArray payload = payloadFromSendArea(&hexOk);
    if (sendHexCheck->isChecked() && !hexOk) {
        QMessageBox::warning(this, tr("串口"), tr("HEX 格式无效，请使用成对的十六进制字符（如 48 65 6C 6C 6F）。"));
        return;
    }

    const ssize_t written = write(serialFd, payload.constData(), static_cast<size_t>(payload.size()));
    if (written < 0) {
        QMessageBox::warning(this, tr("串口"), tr("发送失败：%1").arg(strerror(errno)));
        return;
    }
    setSerialStatus(tr("已发送 %1 字节").arg(written));
}

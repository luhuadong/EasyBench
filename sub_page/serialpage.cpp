#include "serialpage.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QScrollBar>
#include <QVBoxLayout>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

SerialRecvThread::SerialRecvThread(QObject *parent)
    : QThread(parent)
{
}

void SerialRecvThread::setSerialPortFd(int fd)
{
    serialFd = fd;
}

void SerialRecvThread::run()
{
    char buf[256];
    while (!isInterruptionRequested()) {
        if (serialFd < 0) {
            msleep(50);
            continue;
        }
        const ssize_t ret = read(serialFd, buf, sizeof(buf));
        if (ret > 0) {
            emit msgReceived(QByteArray(buf, static_cast<int>(ret)));
        } else if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            break;
        } else {
            msleep(20);
        }
    }
}

SerialPage::SerialPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("串口测试"));
    buildUi();
    refreshPortList();

    operationBar->firstButton()->setEnabled(false);
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    recvThread = new SerialRecvThread(this);
    connect(recvThread, &SerialRecvThread::msgReceived, this, &SerialPage::showRecvData);
}

SerialPage::~SerialPage() = default;

void SerialPage::buildUi()
{
    QWidget *content = contentArea();

    portGroup = new QGroupBox(tr("串口设置"), content);
    serialPortBox = new QComboBox(portGroup);
    serialPortBox->setMinimumWidth(220);
    refreshPortBtn = new QPushButton(tr("刷新"), portGroup);
    refreshPortBtn->setObjectName(QStringLiteral("functionBtn_small"));

    baudRateBox = new QComboBox(portGroup);
    baudRateBox->addItem(QStringLiteral("9600"), 9600);
    baudRateBox->addItem(QStringLiteral("19200"), 19200);
    baudRateBox->addItem(QStringLiteral("38400"), 38400);
    baudRateBox->addItem(QStringLiteral("57600"), 57600);
    baudRateBox->addItem(QStringLiteral("115200"), 115200);
    baudRateBox->setCurrentIndex(0);

    dataBitsBox = new QComboBox(portGroup);
    dataBitsBox->addItem(QStringLiteral("8"), 8);
    dataBitsBox->addItem(QStringLiteral("7"), 7);
    dataBitsBox->setCurrentIndex(0);

    parityBox = new QComboBox(portGroup);
    parityBox->addItem(tr("无"), QStringLiteral("none"));
    parityBox->addItem(tr("偶"), QStringLiteral("even"));
    parityBox->addItem(tr("奇"), QStringLiteral("odd"));

    stopBitsBox = new QComboBox(portGroup);
    stopBitsBox->addItem(QStringLiteral("1"), 1);
    stopBitsBox->addItem(QStringLiteral("2"), 2);

    openBtn = new QPushButton(tr("打开"), portGroup);
    openBtn->setObjectName(QStringLiteral("functionBtn_small"));

    QHBoxLayout *portRow = new QHBoxLayout;
    portRow->setContentsMargins(0, 0, 0, 0);
    portRow->setSpacing(8);
    portRow->addWidget(serialPortBox, 1);
    portRow->addWidget(refreshPortBtn);

    QFormLayout *portForm = new QFormLayout;
    portForm->setContentsMargins(12, 16, 12, 12);
    portForm->addRow(tr("串口"), portRow);
    portForm->addRow(tr("波特率"), baudRateBox);
    portForm->addRow(tr("数据位"), dataBitsBox);
    portForm->addRow(tr("校验"), parityBox);
    portForm->addRow(tr("停止位"), stopBitsBox);
    portForm->addRow(QString(), openBtn);
    portGroup->setLayout(portForm);

    echoGroup = new QGroupBox(tr("收发测试"), content);
    sendArea = new QTextEdit(echoGroup);
    sendArea->setPlainText(QStringLiteral("Hello from EasyBench"));
    sendArea->setMaximumHeight(100);
    recvArea = new QTextEdit(echoGroup);
    recvArea->setReadOnly(true);
    sendBtn = new QPushButton(tr("发送"), echoGroup);
    sendBtn->setObjectName(QStringLiteral("functionBtn_small"));
    sendClearBtn = new QPushButton(tr("清空发送"), echoGroup);
    sendClearBtn->setObjectName(QStringLiteral("functionBtn_small"));
    recvClearBtn = new QPushButton(tr("清空接收"), echoGroup);
    recvClearBtn->setObjectName(QStringLiteral("functionBtn_small"));
    statusLabel = new QLabel(tr("未打开"), echoGroup);
    statusLabel->setWordWrap(true);

    QHBoxLayout *sendBtnRow = new QHBoxLayout;
    sendBtnRow->addWidget(sendBtn);
    sendBtnRow->addWidget(sendClearBtn);
    sendBtnRow->addStretch();

    QHBoxLayout *recvBtnRow = new QHBoxLayout;
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
    echoLayout->addWidget(statusLabel);

    QHBoxLayout *pageLayout = new QHBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(16);
    pageLayout->addWidget(portGroup, 0);
    pageLayout->addWidget(echoGroup, 1);

    echoGroup->setEnabled(false);

    connect(refreshPortBtn, &QPushButton::clicked, this, &SerialPage::refreshPortList);
    connect(serialPortBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPage::onPortChanged);
    connect(openBtn, &QPushButton::clicked, this, &SerialPage::openSerialPort);
    connect(sendBtn, &QPushButton::clicked, this, &SerialPage::sendSerialData);
    connect(sendClearBtn, &QPushButton::clicked, this, &SerialPage::clearSendArea);
    connect(recvClearBtn, &QPushButton::clicked, this, &SerialPage::clearRecvArea);
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
        statusLabel->setText(tr("未检测到串口设备"));
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
}

QString SerialPage::currentPortPath() const
{
    return serialPortBox->currentText();
}

int SerialPage::currentBaudRate() const
{
    return baudRateBox->currentData().toInt();
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
        statusLabel->setText(tr("stty 配置失败：%1").arg(path));
    }
}

void SerialPage::applyParameters()
{
    applySerialPortStty();
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
        recvThread->setSerialPortFd(serialFd);
        recvThread->start();
        echoGroup->setEnabled(true);
        openBtn->setText(tr("关闭"));
        statusLabel->setText(tr("已打开 %1，波特率 %2").arg(path).arg(currentBaudRate()));
    } else {
        recvThread->requestInterruption();
        recvThread->wait(2000);
        if (serialFd >= 0) {
            ::close(serialFd);
            serialFd = -1;
        }
        recvThread->setSerialPortFd(-1);
        echoGroup->setEnabled(false);
        openBtn->setText(tr("打开"));
        statusLabel->setText(tr("已关闭"));
    }
}

void SerialPage::showRecvData(const QByteArray &data)
{
    recvArea->moveCursor(QTextCursor::End);
    recvArea->insertPlainText(QString::fromUtf8(data));
    recvArea->verticalScrollBar()->setValue(recvArea->verticalScrollBar()->maximum());
}

void SerialPage::clearSendArea()
{
    sendArea->clear();
}

void SerialPage::clearRecvArea()
{
    recvArea->clear();
}

void SerialPage::sendSerialData()
{
    const QString path = currentPortPath();
    if (serialFd < 0) {
        QMessageBox::information(this, tr("串口"), tr("请先打开串口。"));
        return;
    }
    const QByteArray payload = sendArea->toPlainText().toUtf8();
    const ssize_t written = write(serialFd, payload.constData(), static_cast<size_t>(payload.size()));
    if (written < 0) {
        QMessageBox::warning(this, tr("串口"), tr("发送失败：%1").arg(strerror(errno)));
        return;
    }
    statusLabel->setText(tr("已发送 %1 字节").arg(written));
}

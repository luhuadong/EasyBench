#ifndef SERIALPAGE_H
#define SERIALPAGE_H

#include "widgets/pagewidget.h"
#include "eb_common.h"

#include <QByteArray>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QThread>
#include <QTimer>

#include <atomic>

class SerialRecvThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialRecvThread(QObject *parent = nullptr);
    void setSerialPortFd(int fd);
    void setDiscardIncoming(bool discard);

signals:
    void msgReceived(const QByteArray &data);

protected:
    void run() override;

private:
    int serialFd = -1;
    std::atomic<bool> discardIncoming{false};
};

/** 串口参数默认值（波特率等），供 UI 与 stty 配置共用 */
namespace SerialParam {

struct ParityOption {
    const char *sttyValue;
};

constexpr int kBaudRates[] = {
    9600, 19200, 38400, 57600, 100000, 115200, 230400, 460800, 921600, 1500000,
};
constexpr int kBaudRateCount = sizeof(kBaudRates) / sizeof(kBaudRates[0]);

constexpr int kDataBits[] = {8, 7};
constexpr int kDataBitsCount = sizeof(kDataBits) / sizeof(kDataBits[0]);

constexpr ParityOption kParityOptions[] = {
    {"none"},
    {"even"},
    {"odd"},
};
constexpr int kParityCount = sizeof(kParityOptions) / sizeof(kParityOptions[0]);

constexpr int kStopBits[] = {1, 2};
constexpr int kStopBitsCount = sizeof(kStopBits) / sizeof(kStopBits[0]);

} // namespace SerialParam

class SerialPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SerialPage(EbOptions *options, QWidget *parent = nullptr);
    ~SerialPage();

    QString defaultStatusHint() const override;

private slots:
    void refreshPortList();
    void onPortChanged(int index);
    void openSerialPort();
    void applyParameters();
    void clearSendArea();
    void clearRecvArea();
    void sendSerialData();
    void appendRecvChunk(const QByteArray &data);
    void flushRecvDisplay();
    void onRecvPauseToggled(bool paused);

private:
    void trimRecvDocument();
    bool recvAutoScroll() const;
    void buildUi();
    void populateParamCombos();
    void applySerialPortStty();
    void setSerialStatus(const QString &text);
    QString currentPortPath() const;
    int currentBaudRate() const;
    QByteArray payloadFromSendArea(bool *hexOk) const;
    QString formatRecvData(const QByteArray &data) const;

    int serialFd = -1;
    SerialRecvThread *recvThread = nullptr;
    QTimer *recvFlushTimer = nullptr;
    QByteArray recvPending;

    QGroupBox *portGroup = nullptr;
    QComboBox *serialPortBox = nullptr;
    QPushButton *refreshPortBtn = nullptr;
    QComboBox *baudRateBox = nullptr;
    QComboBox *dataBitsBox = nullptr;
    QComboBox *parityBox = nullptr;
    QComboBox *stopBitsBox = nullptr;
    QPushButton *openBtn = nullptr;

    QGroupBox *echoGroup = nullptr;
    QTextEdit *sendArea = nullptr;
    QPlainTextEdit *recvArea = nullptr;
    QCheckBox *sendHexCheck = nullptr;
    QCheckBox *recvHexCheck = nullptr;
    QCheckBox *recvPauseCheck = nullptr;
    QPushButton *sendBtn = nullptr;
    QPushButton *sendClearBtn = nullptr;
    QPushButton *recvClearBtn = nullptr;
};

#endif /* SERIALPAGE_H */

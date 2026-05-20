#ifndef SERIALPAGE_H
#define SERIALPAGE_H

#include "custom_widget/pagewidget.h"
#include "eb_common.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QThread>

class SerialRecvThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialRecvThread(QObject *parent = nullptr);
    void setSerialPortFd(int fd);

signals:
    void msgReceived(const QByteArray &data);

protected:
    void run() override;

private:
    int serialFd = -1;
};

class SerialPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SerialPage(EbOptions *options, QWidget *parent = nullptr);
    ~SerialPage();

private slots:
    void refreshPortList();
    void onPortChanged(int index);
    void openSerialPort();
    void applyParameters();
    void clearSendArea();
    void clearRecvArea();
    void sendSerialData();
    void showRecvData(const QByteArray &data);

private:
    void buildUi();
    void applySerialPortStty();
    QString currentPortPath() const;
    int currentBaudRate() const;

    int serialFd = -1;
    SerialRecvThread *recvThread = nullptr;

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
    QTextEdit *recvArea = nullptr;
    QPushButton *sendBtn = nullptr;
    QPushButton *sendClearBtn = nullptr;
    QPushButton *recvClearBtn = nullptr;
    QLabel *statusLabel = nullptr;
};

#endif /* SERIALPAGE_H */

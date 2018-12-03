#ifndef SERIALPORTPAGE_H
#define SERIALPORTPAGE_H

#include "custom_widget/pagewidget.h"
#include "gyt_common.h"

#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QThread>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>

class SerialRecvThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialRecvThread(QWidget *parent=0);
    void setSerialPortFd(int fd);

signals:
    void msgReceived(const QString msg);

private:
    int serialFd;

protected:
    void run();
};

class SerialPortPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SerialPortPage(QWidget *parent = 0);


signals:

private slots:
    void showRecvData(const QString msg);
    void openSerialPort();
    void applyParameters();
    void clearSendArea();
    void clearRecvArea();
    void sendSerialData();
    void changeSerialPort(const QString &text);

private:
    typedef enum BaudRate
    {
        Baud1200    = 1200,
        Baud2400    = 2400,
        Baud4800    = 4800,
        Baud9600    = 9600,
        Baud19200   = 19200,
        Baud38400   = 38400,
        Baud57600   = 57600,
        Baud115200  = 115200,
        UnknownBaud = -1

    } BaudRate;

    typedef enum DataBits
    {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1

    } Databits;

    typedef enum Direction
    {
        Input  = 1,
        Output = 2,
        AllDirections = Input | Output

    } Direction;

    typedef enum FlowControl
    {
        NoFlowControl   = 0,
        HardwareControl = 1,
        SoftwareControl = 2,
        UnknownFlowControl = -1

    } FlowControl;

    typedef enum Parity
    {
        NoParity     = 0,
        EvenParity   = 2,
        OddParity    = 3,
        SpaceParity  = 4,
        MarkParity   = 5,
        UnknowParity = -1

    } Parity;

    typedef enum StopBits
    {
        OneStop         = 1,
        OneAndHalfStop  = 3,
        TwoStop         = 2,
        UnknownStopBits = -1

    } StopBits;

    typedef enum SerialPortError
    {
        NoError                   = 0,
        DeviceNotFoundError       = 1,
        PermissionError           = 2,
        OpenError                 = 3,
        NotOpenError              = 13,
        ParityError               = 4,
        FramingError              = 5,
        BreakConditionError       = 6,
        WriteError                = 7,
        ReadError                 = 8,
        ResourceError             = 9,
        UnsupportedOperationError = 10,
        TimeoutError              = 12,
        UnknownError              = 11

    } SerialPortError;

    int serialFd;
    SerialRecvThread *thread;


    QLabel *label;

    QGroupBox *serialPortGroup;
    QLabel    *serialPortLabel;
    QComboBox *serialPortBox;
    QPushButton *openBtn;

    QGroupBox *parametersGroup;
    QLabel    *baudRateLabel;
    QComboBox *baudRateBox;
    QLabel    *dataBitsLabel;
    QComboBox *dataBitsBox;
    QLabel    *parityLabel;
    QComboBox *parityBox;
    QLabel    *stopBitsLabel;
    QComboBox *stopBitsBox;
    QLabel    *flowControlLabel;
    QComboBox *flowControlBox;
    QPushButton *applyBtn;

    QGroupBox *echoTestGroup;
    QTextEdit *sendArea;
    QTextEdit *recvArea;
    QPushButton *sendClearBtn;
    QPushButton *recvClearBtn;
    QPushButton *testBtn;




};

#endif // SERIALPORTPAGE_H

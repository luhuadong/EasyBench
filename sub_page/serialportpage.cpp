#include "serialportpage.h"
#include "gyt_common.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMessageBox>

SerialRecvThread::SerialRecvThread(QWidget *parent)
{

}

void SerialRecvThread::setSerialPortFd(int fd)
{
    serialFd = fd;
}

void SerialRecvThread::run()
{
    char buf[128];
    int ret;

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = read(serialFd, buf, sizeof(buf));
        if(ret > 0) {

            if(buf[0] == '\n' || buf[0] == '\r') {
                continue;
            }

            int len = strlen(buf);
            buf[len-1] = '\0';
            qDebug() << tr(gSerialPortStr) << tr(" : %1 ").arg(len) << tr(buf);
            //((SerialPortPage *)parent())->showRecvData(buf);
            //qobject_cast<SerialPortPage *>(this->parent())->showRecvData(buf);
            QString msg(buf);
            emit msgReceived(msg);
        }
        else
            continue;
    }

}

SerialPortPage::SerialPortPage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Serial Port Test"));

    /* Select Serial Port Box */

    serialPortGroup = new QGroupBox(tr("Select Serial Port"), this);
    serialPortGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    serialPortLabel = new QLabel(tr("Current port : "), serialPortGroup);
    serialPortLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    serialPortBox = new QComboBox(serialPortGroup);
    //serialPortBox->addItem(QString(gSerialPortStr));
    serialPortBox->addItem(tr("/dev/ttymxc0"));
    serialPortBox->addItem(tr("/dev/ttymxc1"));
    serialPortBox->addItem(tr("/dev/ttymxc2"));
    serialPortBox->addItem(tr("/dev/ttymxc3"));
    serialPortBox->addItem(tr("/dev/ttymxc4"));
    serialPortBox->setCurrentIndex(1);
    strcpy(gSerialPortStr, serialPortBox->currentText().toLatin1().data());
    openBtn = new QPushButton(tr("Open"), serialPortGroup);
    openBtn->setObjectName("functionBtn_small");
    openBtn->setFixedSize(80, 30);

    QGridLayout *serialPortLayout = new QGridLayout;
    serialPortLayout->setMargin(20);
    serialPortLayout->addWidget(serialPortLabel, 0, 0);
    serialPortLayout->addWidget(serialPortBox, 0, 1);
    serialPortLayout->addWidget(openBtn, 1, 1, Qt::AlignRight);
    serialPortGroup->setLayout(serialPortLayout);

    /* Select Parameters Box */

    parametersGroup = new QGroupBox(tr("Select Parameters"), this);
    parametersGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    baudRateLabel = new QLabel(tr("BaudRate : "), parametersGroup);
    baudRateLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    baudRateBox = new QComboBox(parametersGroup);
    baudRateBox->addItem(tr("9600"), Baud9600);
    baudRateBox->addItem(tr("19200"), Baud19200);
    baudRateBox->addItem(tr("38400"), Baud38400);
    baudRateBox->addItem(tr("115200"), Baud115200);

    dataBitsLabel = new QLabel(tr("Data bits : "), parametersGroup);
    dataBitsLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    dataBitsBox = new QComboBox(parametersGroup);
    dataBitsBox->addItem(tr("5"), Data5);
    dataBitsBox->addItem(tr("6"), Data6);
    dataBitsBox->addItem(tr("7"), Data7);
    dataBitsBox->addItem(tr("8"), Data8);
    //dataBitsBox->setCurrentIndex(Data8);
    dataBitsBox->setCurrentIndex(3);


    parityLabel = new QLabel(tr("Parity : "), parametersGroup);
    parityLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    parityBox = new QComboBox(parametersGroup);
    parityBox->addItem(tr("None"), NoParity);
    parityBox->addItem(tr("Even"), EvenParity);
    parityBox->addItem(tr("Odd"), OddParity);
    parityBox->addItem(tr("Mark"), MarkParity);
    parityBox->addItem(tr("Space"), SpaceParity);

    stopBitsLabel = new QLabel(tr("Stop bits : "), parametersGroup);
    stopBitsLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    stopBitsBox = new QComboBox(parametersGroup);
    stopBitsBox->addItem(tr("1"), OneStop);
    stopBitsBox->addItem(tr("2"), TwoStop);

    flowControlLabel = new QLabel(tr("Flow control : "), parametersGroup);
    flowControlLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    flowControlBox = new QComboBox(parametersGroup);
    flowControlBox->addItem(tr("None"), NoFlowControl);
    flowControlBox->addItem(tr("RTS/CTS"), HardwareControl);
    flowControlBox->addItem(tr("XON/XOFF"), SoftwareControl);

    applyBtn = new QPushButton(tr("Apply"), parametersGroup);
    applyBtn->setObjectName("functionBtn_small");
    applyBtn->setFixedSize(80, 30);

    QGridLayout *parametersLayout = new QGridLayout;
    parametersLayout->setMargin(20);
    parametersLayout->addWidget(baudRateLabel, 0, 0);
    parametersLayout->addWidget(baudRateBox, 0, 1);
    parametersLayout->addWidget(dataBitsLabel, 1, 0);
    parametersLayout->addWidget(dataBitsBox, 1, 1);
    parametersLayout->addWidget(parityLabel, 2, 0);
    parametersLayout->addWidget(parityBox, 2, 1);
    parametersLayout->addWidget(stopBitsLabel, 3, 0);
    parametersLayout->addWidget(stopBitsBox, 3, 1);
    parametersLayout->addWidget(flowControlLabel, 4, 0);
    parametersLayout->addWidget(flowControlBox, 4, 1);
    parametersLayout->addWidget(applyBtn, 5, 1, Qt::AlignRight);
    parametersGroup->setLayout(parametersLayout);

    /* Serial Port Echo Test */
    echoTestGroup = new QGroupBox(tr("Serial Port Echo Test"), this);
    echoTestGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    sendArea = new QTextEdit(echoTestGroup);
    sendArea->setMinimumWidth(360);

    recvArea = new QTextEdit(echoTestGroup);
    recvArea->setMinimumWidth(360);
    recvArea->setReadOnly(true);

    sendClearBtn = new QPushButton(tr("Clear"), echoTestGroup);
    sendClearBtn->setObjectName("functionBtn_small");
    sendClearBtn->setFixedSize(80, 30);

    recvClearBtn = new QPushButton(tr("Clear"), echoTestGroup);
    recvClearBtn->setObjectName("functionBtn_small");
    recvClearBtn->setFixedSize(80, 30);

    testBtn = new QPushButton(tr("Send"), echoTestGroup);
    testBtn->setObjectName("functionBtn_small");
    testBtn->setFixedSize(80, 30);

    QGridLayout *echoTestLayout = new QGridLayout;
    echoTestLayout->setMargin(20);
    echoTestLayout->addWidget(sendArea, 0, 0, 1, 2);
    echoTestLayout->addWidget(sendClearBtn, 1, 0, 1, 1);
    echoTestLayout->addWidget(testBtn, 1, 1, 1, 1);
    echoTestLayout->addWidget(recvArea, 2, 0, 1, 2);
    echoTestLayout->addWidget(recvClearBtn, 3, 0, 1, 1);
    echoTestGroup->setLayout(echoTestLayout);

    serialPortGroup->setGeometry(40, 96+30, 360, 140);
    parametersGroup->setGeometry(40, 96+200, 360, 320);
    echoTestGroup->setGeometry(40+360+40, 96+30, 420, 140+30+320);
    echoTestGroup->setEnabled(false);

    connect(openBtn, SIGNAL(clicked()), this, SLOT(openSerialPort()));
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyParameters()));
    connect(sendClearBtn, SIGNAL(clicked()), this, SLOT(clearSendArea()));
    connect(recvClearBtn, SIGNAL(clicked()), this, SLOT(clearRecvArea()));
    connect(testBtn, SIGNAL(clicked()), this, SLOT(sendSerialData()));
    connect(serialPortBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeSerialPort(QString)));

    thread = new SerialRecvThread(this);
    connect(thread, SIGNAL(msgReceived(QString)), this, SLOT(showRecvData(QString)));

    /*
    if(NoError == openSerialPort())
    {
        qDebug() << tr("open success");
        thread = new SerialRecvThread(serialFd, this);
        //connect(thread, SIGNAL(msgReceived(const char*)), this, SLOT(showRecvData(const char*)));
        connect(thread, SIGNAL(msgReceived(QString)), this, SLOT(showRecvData(QString)));
        thread->start();
    }
    */

    char cmd[128];
    sprintf(cmd, "stty -F %s speed 9600 cs8 -parenb -cstopb -echo", gSerialPortStr);
    system(cmd);

}

void SerialPortPage::showRecvData(const QString msg)
{

    //QString text(msg);
    recvArea->setText(msg);
    qDebug() << tr("showRecvData : ") << msg;
}

void SerialPortPage::openSerialPort()
{
    qDebug() << tr("openBtn clicked");

    if(openBtn->text() == QString("Open"))
    {
        qDebug() << tr("open");

        serialFd = open(gSerialPortStr, O_RDWR | O_NOCTTY | O_NDELAY);
        if(serialFd == -1) {

            QMessageBox msgBox;
            msgBox.setText(tr("Open %1 failed.").arg(gSerialPortStr));
            msgBox.exec();
            //return OpenError;
        }
        else
        {
            thread->setSerialPortFd(serialFd);
            thread->start();
            echoTestGroup->setEnabled(true);
            applyBtn->setEnabled(false);
            openBtn->setText(tr("Close"));

            /*
            QMessageBox msgBox;
            msgBox.setText(tr("Open %1 successfully.").arg(gSerialPortStr));
            msgBox.exec();
            */
        }


        //return NoError;
    }
    else if(openBtn->text() == QString("Close"))
    {
        qDebug() << tr("close");

        thread->quit();

        //close(serialFd);
        //::close(serialFd);

        echoTestGroup->setEnabled(false);
        applyBtn->setEnabled(true);
        openBtn->setText(tr("Open"));
    }
}

void SerialPortPage::changeSerialPort(const QString &text)
{
    qDebug() << tr("changeSerialPort: ") << text;
    strcpy(gSerialPortStr, text.toLatin1().data());
    qDebug() << tr("change Serial Port ") << QString(gSerialPortStr);
}

void SerialPortPage::applyParameters()
{
    char cmd[128];
    sprintf(cmd, "stty -F %s speed 9600 cs8 -parenb -cstopb -echo", gSerialPortStr);
    system(cmd);

    QMessageBox msgBox;
    msgBox.setText(tr("Configure parameters of %1 successfully.").arg(gSerialPortStr));
    msgBox.exec();
}

void SerialPortPage::clearSendArea()
{
    sendArea->clear();
    sendArea->setFocus();
}

void SerialPortPage::clearRecvArea()
{
    recvArea->clear();
}

void SerialPortPage::sendSerialData()
{
    char cmd[128];
    QString sendStr = sendArea->toPlainText();
    // echo string should be add option '-e'
    sprintf(cmd, "echo -e \"%s\" > %s", sendStr.toLatin1().data(), gSerialPortStr);
    system(cmd);

}

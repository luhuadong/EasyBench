#include "systempage.h"
#include "gyt_common.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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
            //((SystemPage *)parent())->showRecvData(buf);
            //qobject_cast<SystemPage *>(this->parent())->showRecvData(buf);
            QString msg(buf);
            emit msgReceived(msg);
        }
        else
            continue;
    }
}

SystemPage::SystemPage(QWidget *parent) :
    PageWidget(parent)
{
    //setTitleLabelText(tr("Serial Port Test"));
    setTitleLabelText(tr("系统功能测试"));

    /* Select Serial Port Box */
    this->initSerialPortArea();


    /* RTC Group */

    rtcGroup = new QGroupBox(tr("实时时钟"), this);
    rtcGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    rtcName = new QLabel(tr("设备型号 : None"), rtcGroup);
    rtcName->setFont(QFont("Courier", 10, QFont::Normal));
    rtcDateTime = new QLabel(tr("RTC时间 : None"), rtcGroup);
    rtcDateTime->setFont(QFont("Courier", 10, QFont::Normal));
    sysDateTime = new QLabel(tr("系统时间 : None"), rtcGroup);
    sysDateTime->setFont(QFont("Courier", 10, QFont::Normal));

    QVBoxLayout *rtcLayout = new QVBoxLayout;
    rtcLayout->setMargin(10);
    rtcLayout->setSpacing(10);
    rtcLayout->addWidget(rtcName);
    rtcLayout->addWidget(rtcDateTime);
    rtcLayout->addWidget(sysDateTime);
    rtcGroup->setLayout(rtcLayout);

    rtcGroup->setGeometry(40+380+40, 96+30, 400, 200);

    char buf[32];
    memset(buf, 0, sizeof(buf));

    FILE *fstream = NULL;

    if(NULL == (fstream = popen("cat /sys/class/rtc/rtc0/name", "r"))) {

        rtcName->setText("设备型号 : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        rtcName->setText(QString("设备型号 : %1").arg(QString(buf)));
    }
    pclose(fstream);



    /* SATA Group */

    sdiskGroup = new QGroupBox(tr("SATA读写测试"), this);
    sdiskGroup->setFont(QFont("Helvetica", 12, QFont::Bold));
    sdiskGroup->setGeometry(40+380+40, 96+30+200+30, 400, 270);

    sdiskArea = new QTextEdit(sdiskGroup);
    sdiskArea->setReadOnly(true);

    readTestBtn = new QPushButton(tr("读取"), sdiskGroup);
    writeTestBtn = new QPushButton(tr("写入"), sdiskGroup);
    readTestBtn->setObjectName("functionBtn_small");
    readTestBtn->setFixedSize(120, 30);
    writeTestBtn->setObjectName("functionBtn_small");
    writeTestBtn->setFixedSize(120, 30);

    QHBoxLayout *sdiskBtnLayout = new QHBoxLayout;
    sdiskBtnLayout->setSpacing(10);
    sdiskBtnLayout->addWidget(readTestBtn);
    sdiskBtnLayout->addWidget(writeTestBtn);

    QVBoxLayout *sdiskLayout = new QVBoxLayout;
    sdiskLayout->setMargin(10);
    sdiskLayout->setSpacing(10);
    sdiskLayout->addWidget(sdiskArea);
    sdiskLayout->addLayout(sdiskBtnLayout);
    sdiskGroup->setLayout(sdiskLayout);

    connect(readTestBtn, SIGNAL(clicked()), this, SLOT(readTestBtnOnClicked()));
    connect(writeTestBtn, SIGNAL(clicked()), this, SLOT(writeTestBtnOnClicked()));


    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(on_updateTimer_timeout()));
    updateTimer->start(1000);


}

void SystemPage::initSerialPortArea()
{
    serialPortGroup = new QGroupBox(tr("串口设置"), this);
    serialPortGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    serialPortLabel = new QLabel(tr("当前串口 : "), serialPortGroup);
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

    baudRateLabel = new QLabel(tr("波特率 : "), serialPortGroup);
    baudRateLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    baudRateBox = new QComboBox(serialPortGroup);
    baudRateBox->addItem(tr("9600"), Baud9600);
    baudRateBox->addItem(tr("19200"), Baud19200);
    baudRateBox->addItem(tr("38400"), Baud38400);
    baudRateBox->addItem(tr("115200"), Baud115200);

    /*
    dataBitsLabel = new QLabel(tr("数据位 : "), serialPortGroup);
    dataBitsLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    dataBitsBox = new QComboBox(serialPortGroup);
    dataBitsBox->addItem(tr("5"), Data5);
    dataBitsBox->addItem(tr("6"), Data6);
    dataBitsBox->addItem(tr("7"), Data7);
    dataBitsBox->addItem(tr("8"), Data8);
    //dataBitsBox->setCurrentIndex(Data8);
    dataBitsBox->setCurrentIndex(3);


    parityLabel = new QLabel(tr("奇偶校验 : "), serialPortGroup);
    parityLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    parityBox = new QComboBox(serialPortGroup);
    parityBox->addItem(tr("None"), NoParity);
    parityBox->addItem(tr("Even"), EvenParity);
    parityBox->addItem(tr("Odd"), OddParity);
    parityBox->addItem(tr("Mark"), MarkParity);
    parityBox->addItem(tr("Space"), SpaceParity);

    stopBitsLabel = new QLabel(tr("停止位 : "), serialPortGroup);
    stopBitsLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    stopBitsBox = new QComboBox(serialPortGroup);
    stopBitsBox->addItem(tr("1"), OneStop);
    stopBitsBox->addItem(tr("2"), TwoStop);


    flowControlLabel = new QLabel(tr("流控 : "), serialPortGroup);
    flowControlLabel->setFont(QFont("Helvetica", 12, QFont::Normal));
    flowControlBox = new QComboBox(serialPortGroup);
    flowControlBox->addItem(tr("None"), NoFlowControl);
    flowControlBox->addItem(tr("RTS/CTS"), HardwareControl);
    flowControlBox->addItem(tr("XON/XOFF"), SoftwareControl);
    */

    applyBtn = new QPushButton(tr("应用"), serialPortGroup);
    applyBtn->setObjectName("functionBtn_small");
    applyBtn->setFixedSize(80, 30);
    applyBtn->setVisible(false); // 自动设置串口参数，这样就可以去掉Apply按钮

    openBtn = new QPushButton(tr("打开"), serialPortGroup);
    openBtn->setObjectName("functionBtn_small");
    openBtn->setFixedSize(80, 30);



    QGridLayout *serialPortLayout = new QGridLayout;
    //serialPortLayout->setMargin(10);
    serialPortLayout->addWidget(serialPortLabel, 0, 0);
    serialPortLayout->addWidget(serialPortBox, 0, 1);
    serialPortLayout->addWidget(baudRateLabel, 1, 0);
    serialPortLayout->addWidget(baudRateBox, 1, 1);
    //serialPortLayout->addWidget(dataBitsLabel, 2, 0);
    //serialPortLayout->addWidget(dataBitsBox, 2, 1);
    //serialPortLayout->addWidget(parityLabel, 3, 0);
    //serialPortLayout->addWidget(parityBox, 3, 1);
    //serialPortLayout->addWidget(stopBitsLabel, 4, 0);
    //serialPortLayout->addWidget(stopBitsBox, 4, 1);
    //serialPortLayout->addWidget(flowControlLabel, 5, 0);
    //serialPortLayout->addWidget(flowControlBox, 5, 1);
    serialPortLayout->addWidget(applyBtn, 5, 0, Qt::AlignRight);
    serialPortLayout->addWidget(openBtn, 5, 1, Qt::AlignRight);
    serialPortGroup->setLayout(serialPortLayout);

    /* Select Parameters Box */

#if 0
    parametersGroup = new QGroupBox(tr("Select Parameters"), this);
    parametersGroup->setFont(QFont("Helvetica", 12, QFont::Bold));





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
#endif

    /* Serial Port Echo Test */
    echoTestGroup = new QGroupBox(tr("串口环回测试"), this);
    echoTestGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    sendArea = new QTextEdit(echoTestGroup);
    sendArea->setMinimumWidth(280);

    recvArea = new QTextEdit(echoTestGroup);
    recvArea->setMinimumWidth(280);
    recvArea->setReadOnly(true);

    sendClearBtn = new QPushButton(tr("清除"), echoTestGroup);
    sendClearBtn->setObjectName("functionBtn_small");
    sendClearBtn->setFixedSize(80, 30);

    recvClearBtn = new QPushButton(tr("清除"), echoTestGroup);
    recvClearBtn->setObjectName("functionBtn_small");
    recvClearBtn->setFixedSize(80, 30);

    testBtn = new QPushButton(tr("发送"), echoTestGroup);
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

    serialPortGroup->setGeometry(40, 96+30, 360, 160);
    //parametersGroup->setGeometry(40, 96+200, 360, 320);
    echoTestGroup->setGeometry(40, 96+30+140+30, 360, 330);
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

void SystemPage::showRecvData(const QString msg)
{

    //QString text(msg);
    recvArea->setText(msg);
    qDebug() << tr("showRecvData : ") << msg;
}

void SystemPage::openSerialPort()
{
    qDebug() << tr("openBtn clicked");

    if(openBtn->text() == QString("打开"))
    {
        qDebug() << tr("open");
        this->applyParameters(); // 自动设置串口参数，这样就可以去掉Apply按钮

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
            openBtn->setText(tr("关闭"));

            /*
            QMessageBox msgBox;
            msgBox.setText(tr("Open %1 successfully.").arg(gSerialPortStr));
            msgBox.exec();
            */
        }


        //return NoError;
    }
    else if(openBtn->text() == QString("关闭"))
    {
        qDebug() << tr("close");

        thread->quit();

        //close(serialFd);
        //::close(serialFd);

        echoTestGroup->setEnabled(false);
        applyBtn->setEnabled(true);
        openBtn->setText(tr("打开"));
    }
}

void SystemPage::changeSerialPort(const QString &text)
{
    qDebug() << tr("changeSerialPort: ") << text;
    strcpy(gSerialPortStr, text.toLatin1().data());
    qDebug() << tr("change Serial Port ") << QString(gSerialPortStr);
}

void SystemPage::applyParameters()
{
    char cmd[128];
    sprintf(cmd, "stty -F %s speed 9600 cs8 -parenb -cstopb -echo", gSerialPortStr);
    system(cmd);
#if 0
    QMessageBox msgBox;
    msgBox.setText(tr("Configure parameters of %1 successfully.").arg(gSerialPortStr));
    msgBox.exec();
#endif
}

void SystemPage::clearSendArea()
{
    sendArea->clear();
    sendArea->setFocus();
}

void SystemPage::clearRecvArea()
{
    recvArea->clear();
}

void SystemPage::sendSerialData()
{
    char cmd[128];
    QString sendStr = sendArea->toPlainText();
    // echo string should be add option '-e'
    sprintf(cmd, "echo -e \"%s\" > %s", sendStr.toLatin1().data(), gSerialPortStr);
    system(cmd);

}


void SystemPage::on_updateTimer_timeout()
{
    static char buf[128];
    memset(buf, 0, sizeof(buf));

    FILE *fstream = NULL;


    if(NULL == (fstream = popen("hwclock -u", "r"))) {

        rtcDateTime->setText("RTC时间 : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        rtcDateTime->setText(QString("RTC时间 : %1").arg(QString(buf)));
    }

    memset(buf, 0, sizeof(buf));


    if(NULL == (fstream = popen("date", "r"))) {

        sysDateTime->setText("系统时间 : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        sysDateTime->setText(QString("系统时间 : %1").arg(QString(buf)));
    }

    pclose(fstream);
}

void SystemPage::readTestBtnOnClicked()
{
    //sdiskArea->setText("SATA disk read testing...");
    sdiskArea->append(QString("SATA disk read testing..."));
    qDebug("SATA disk read testing...");

    char cmd[128];
    char buf[256];

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    sprintf(cmd, "dd if=/dev/sda of=/dev/null bs=1M count=100");


    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        sprintf(buf, "Execute command failed: %s", strerror(errno));
        sdiskArea->setText(QString(buf));
        return ;
    }

    fread(buf, sizeof(buf), 1, fstream);
    qDebug(buf);

    //sdiskArea->clear();
    /*
    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        qDebug("====== content ======");
        qDebug(buf);
        int len = strlen(buf);
        if(buf[len-1] == '\n' || buf[len-1] == '\r')
        {
            buf[len-1] = '\0';
        }
        sdiskArea->append(QString(buf));
    }
    */
    pclose(fstream);
    sdiskArea->moveCursor(QTextCursor::Start);
}

void SystemPage::writeTestBtnOnClicked()
{
    //sdiskArea->setText("SATA disk write testing...");
    sdiskArea->append(QString("SATA disk write testing..."));
    qDebug("SATA disk write testing...");

    char cmd[128];
    char buf[256];

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    sprintf(cmd, "dd if=/dev/zero of=/dev/sda bs=1M count=100");


    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        sprintf(buf, "Execute command failed: %s", strerror(errno));
        sdiskArea->setText(QString(buf));
        return ;
    }

    //sdiskArea->clear();
    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        qDebug("====== content ======");
        qDebug(buf);
        int len = strlen(buf);
        if(buf[len-1] == '\n' || buf[len-1] == '\r')
        {
            buf[len-1] = '\0';
        }
        sdiskArea->append(QString(buf));
    }
    pclose(fstream);
    sdiskArea->moveCursor(QTextCursor::Start);
}

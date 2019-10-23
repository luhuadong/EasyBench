#include "networkpage.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QProcess>

#define I210NIC_FILE "I210NIC-origin.otp"


NetworkPage::NetworkPage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("Network Setting"));
    setTitleLabelText(tr("网卡及网络设置"));

    ifList = QNetworkInterface::allInterfaces();

    i210Group = new QGroupBox(tr("I210 eepromARMtool"), this);
    i210Group->setFont(QFont("Helvetica", 14, QFont::Bold));

    macAddrLabel = new QLabel(tr("MAC Address : "), i210Group);
    macAddrLabel->setFont(QFont("Helvetica", 14, QFont::Normal));

    macAddrLine = new QLineEdit(i210Group);
    macAddrLine->setObjectName("inputLineEdit");
    macAddrLine->setFixedHeight(40);
    macAddrLine->setFont(QFont("Times", 18, QFont::Normal));
    /*
    QRegExp rx;
    rx.setPattern(QString("^[0-9A-Fa-f]+$"));
    macAddrLine->setValidator(new QRegExpValidator(rx, this));
    */
    macAddrLine->setInputMask(QString("HH:HH:HH:HH:HH:HH;0"));
    //macAddrLine->setInputMask(QString("XX:XX:XX:XX:XX:XX;0"));
    macAddrLine->setReadOnly(true);
    macAddrLine->setText(tr("00:25:A8:01:7D:B6"));

    editBtn = new QPushButton(tr("Edit"), i210Group);
    editBtn->setObjectName("functionBtn_small");
    editBtn->setFixedSize(60, 40);

    checkBtn = new QPushButton(tr("Check"), i210Group);
    checkBtn->setObjectName("functionBtn_small");
    checkBtn->setFixedSize(100, 40);

    dumpBtn = new QPushButton(tr("Dump"), i210Group);
    dumpBtn->setObjectName("functionBtn_small");
    dumpBtn->setFixedSize(100, 40);

    writeBtn = new QPushButton(tr("Write"), i210Group);
    writeBtn->setObjectName("functionBtn_small");
    writeBtn->setFixedSize(100, 40);

    showArea = new QTextEdit(i210Group);
    showArea->setReadOnly(true);
    //showArea->setFont(QFont("Times", 16, QFont::Normal));
    showArea->setFont(QFont("Georgia", 14, QFont::Normal));


    QHBoxLayout *macAddrLayout = new QHBoxLayout;
    macAddrLayout->setMargin(20);
    macAddrLayout->setSpacing(10);
    macAddrLayout->addWidget(macAddrLabel);
    macAddrLayout->addWidget(macAddrLine);
    macAddrLayout->addWidget(editBtn);

    QHBoxLayout *funcLayout = new QHBoxLayout;
    funcLayout->setMargin(20);
    funcLayout->setSpacing(20);
    funcLayout->addWidget(checkBtn);
    funcLayout->addWidget(dumpBtn);
    funcLayout->addWidget(writeBtn);

    QVBoxLayout *i210Layout = new QVBoxLayout;
    i210Layout->setMargin(20);
    i210Layout->setSpacing(0);
    i210Layout->addLayout(macAddrLayout, 1);
    i210Layout->addLayout(funcLayout, 1);
    i210Layout->addWidget(showArea, 4);

    /*
    QGridLayout *i210Layout = new QGridLayout;
    i210Layout->setMargin(20);
    i210Layout->addWidget(macAddrLabel, 0, 0, 1, 1);
    i210Layout->addWidget(macAddrLine, 0, 1, 1, 1);
    i210Layout->addWidget(editBtn, 0, 2, 1, 1);
    i210Layout->addWidget(checkBtn, 1, 0, 1, 1, Qt::AlignRight);
    i210Layout->addWidget(dumpBtn, 1, 1, 1, 1, Qt::AlignCenter);
    i210Layout->addWidget(writeBtn, 1, 2, 1, 1, Qt::AlignLeft);
    i210Layout->addWidget(showArea, 2, 0, 1, 3);

    */

    i210Group->setLayout(i210Layout);
    i210Group->setGeometry(40, 96+30, 820, 500);

    connect(editBtn, SIGNAL(clicked()), this, SLOT(editBtnOnClicked()));
    connect(checkBtn, SIGNAL(clicked()), this, SLOT(checkBtnOnClicked()));
    connect(dumpBtn, SIGNAL(clicked()), this, SLOT(dumpBtnOnClicked()));
    connect(writeBtn, SIGNAL(clicked()), this, SLOT(writeBtnOnClicked()));

#if LANGUAGE_CHINESE
    macAddrLabel->setText(tr("物理地址 : "));
    editBtn->setText(tr("编辑"));
    checkBtn->setText(tr("检查"));
    dumpBtn->setText(tr("读取"));
    writeBtn->setText(tr("写入"));

#endif

    if(!hasI210OnBoard()) {
        i210Group->setEnabled(false);
    } else {
        QString macStr = getMacAddrFromEditLine();
        qDebug() << tr("getMacAddrFromEditLine = ") << macStr;
    }

}

bool NetworkPage::hasI210OnBoard()
{
    char cmd[128];
    char buf[256];

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    sprintf(cmd, "lspci");

    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        return false;
    }

    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        QString checkStr(buf);
        if(checkStr.contains(QString("I210"))) {
            return true;
        }
    }
    pclose(fstream);
    return false;
}

QString NetworkPage::getMacAddrFromEditLine()
{
    QString macAddrStr = macAddrLine->text();
    MACAddrList = macAddrStr.split(":");

    for(int i=0; i<MACAddrList.count(); i++)
    {
        //qDebug() << tr("MAC[%1]").arg(i) << macAddrList.at(i);
        //MACAddrList.at(i) = MACAddrList.at(i).sprintf("%02x", MACAddrList.at(i).toInt(0, 16)).toUpper();

        QString tmpStr;
        MACAddrList.replace(i, tmpStr.sprintf("%02x", MACAddrList.at(i).toInt(0, 16)).toUpper());

        //qDebug() << tr("MAC[%1]").arg(i) << MACAddrList.at(i);

        if(MACAddrList.at(i).isEmpty()) {
            MACAddr[i] = 0x00;
        }
        else {
            MACAddr[i] = MACAddrList.at(i).toUShort(0, 16);
        }
    }
#if 0
    for(int i=0; i<sizeof(MACAddr)/sizeof(char); i++)
    {
        qDebug() << tr("MAC[%1]").arg(i) << QString::number(MACAddr[i], 16).toUpper();
    }
#endif

    return (MACAddrList.join(":"));
}

void NetworkPage::editBtnOnClicked()
{
    macAddrLine->setReadOnly(false);
    macAddrLine->setFocus();
    macAddrLine->setCursorPosition(0);

}

void NetworkPage::checkBtnOnClicked()
{
    char cmd[128];
    char buf[256];

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    sprintf(cmd, "./eepromARMtool");

    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        sprintf(buf, "Execute command failed: %s", strerror(errno));
        showArea->setText(QString(buf));
        return ;
    }

    showArea->clear();
    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        int len = strlen(buf);
        if(buf[len-1] == '\n' || buf[len-1] == '\r')
        {
            buf[len-1] = '\0';
        }
        showArea->append(QString(buf));
    }
    pclose(fstream);
    showArea->moveCursor(QTextCursor::Start);

}

void NetworkPage::dumpBtnOnClicked()
{
    char cmd[128];
    char buf[256];

    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    sprintf(cmd, "./eepromARMtool -dump -NIC=1");

    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        sprintf(buf, "Execute command failed: %s", strerror(errno));
        showArea->setText(QString(buf));
        return ;
    }

    showArea->clear();
    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        int len = strlen(buf);
        if(buf[len-1] == '\n' || buf[len-1] == '\r')
        {
            buf[len-1] = '\0';
        }
        showArea->append(QString(buf));
    }
    pclose(fstream);
    showArea->append(QString("------------------------------------------------")); // 48 points

    QFile file;
    file.setFileName(QString("./I210NIC.otp"));
    if (!file.exists()) {
        showArea->append(QString("Occur unknown error when reading file."));
        return ;
    }
    else {
        file.readLine();
        file.open(QFile::ReadOnly);
        showArea->append(file.readAll());
        file.close();
    }
    showArea->moveCursor(QTextCursor::Start);

}

bool NetworkPage::generateI210File()
{
    /* Make sure the MAC address */

    QMessageBox msgBox;
    QPushButton *noBtn = msgBox.addButton(tr("NO"), QMessageBox::ActionRole);
    QPushButton *yesBtn = msgBox.addButton(tr("YES"), QMessageBox::ActionRole);

    msgBox.setText(tr("MAC Address: %1\nDo you really want to write the EEPROM ?").arg(getMacAddrFromEditLine()));
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
    msgBox.exec();
    if(msgBox.clickedButton() == noBtn) {
        return false;
    } else if(msgBox.clickedButton() == yesBtn) {

    }

    /* Check whether the file exists or not */

    QFile i210File;
    i210File.setFileName(QString(I210NIC_FILE));
    if(!i210File.exists()) {
        showArea->setText(tr("Error: File %1 not existed.\nPlease check the file.").arg(QString(I210NIC_FILE)));
        return false;
    }

    /* Update MAC address to the file */

    if(!i210File.open(QIODevice::ReadOnly)) {
        showArea->setText(tr("Error: Open file %1 failed.").arg(QString(I210NIC_FILE)));
        return false;
    }

    //QFile tmpFile;
    tmpFile.setFileName(QString("tmp.otp"));
    if(!tmpFile.open(QIODevice::WriteOnly)) {
        showArea->setText(tr("Error: Unkown error, please check memory or flash."));
        return false;
    }

    QString tmpStr;
    int flag = 0;

    while(!i210File.atEnd()) {

        tmpStr = i210File.readLine();
        if(flag == 0) {
            QStringList tmpList = tmpStr.split(" ");
            /*
            tmpList.at(0) = MACAddrList.at(1) + MACAddrList.at(0);
            tmpList.at(1) = MACAddrList.at(3) + MACAddrList.at(2);
            tmpList.at(2) = MACAddrList.at(5) + MACAddrList.at(4);
            */
            tmpList.replace(0, MACAddrList.at(1) + MACAddrList.at(0));
            tmpList.replace(1, MACAddrList.at(3) + MACAddrList.at(2));
            tmpList.replace(2, MACAddrList.at(5) + MACAddrList.at(4));
            tmpStr = tmpList.join(" ");
        }
        flag++;
        tmpFile.write(tmpStr.toLocal8Bit());
    }

    i210File.close();
    tmpFile.close();
}

void NetworkPage::writeBtnOnClicked()
{
    if(!generateI210File()) {
        return ;
    }

    /* Write the file to the NVM of I210 */

    char cmd[128];
    char buf[256];
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));

    //sprintf(cmd, "./eepromARMtool -write -NIC=1 -f=%s", I210NIC_FILE);
    sprintf(cmd, "./eepromARMtool -write -NIC=1 -f=%s", tmpFile.fileName().toLocal8Bit().data());
    //sprintf(cmd, "./eepromARMtool -write -NIC=1 -f=tmp.otp");
    //./eepromARMtool -write -NIC=1 -f=tmp.otp
    qDebug() << tr("cmd : ") << QString(cmd);

    FILE *fstream = NULL;
    if(NULL == (fstream = popen(cmd, "r")))
    {
        sprintf(buf, "Execute command failed: %s", strerror(errno));
        showArea->setText(QString(buf));
        return ;
    }

    int isWriteFinished = 0;

    showArea->clear();
    while(NULL != fgets(buf, sizeof(buf), fstream))
    {
        int len = strlen(buf);
        if(buf[len-1] == '\n' || buf[len-1] == '\r')
        {
            buf[len-1] = '\0';
        }
        showArea->append(QString(buf));
        if(QString(buf).contains(QString("Flash update complete"))) {
            isWriteFinished++;
        }
    }
    pclose(fstream);
    showArea->moveCursor(QTextCursor::Start);

    while(isWriteFinished)
    {
        QMessageBox msgBox;
        QPushButton *noBtn = msgBox.addButton(tr("NO"), QMessageBox::ActionRole);
        QPushButton *yesBtn = msgBox.addButton(tr("YES"), QMessageBox::ActionRole);

        msgBox.setText(tr("Flash update complete\nDo you want to reboot now ?").arg(getMacAddrFromEditLine()));
        msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
        msgBox.exec();
        if(msgBox.clickedButton() == noBtn) {
            return ;
        } else if(msgBox.clickedButton() == yesBtn) {
            system("reboot");
        }
    }
}


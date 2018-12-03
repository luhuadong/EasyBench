#include "aboutpage.h"
#include <QTextCodec>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <errno.h>

AboutPage::AboutPage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Seat Settings"));

    channelStrList_en << tr("LeftPhone") << tr("RightPhone") << tr("Speak") << tr("Radio");
    channelStrList_zh << tr("左电话") << tr("右电话") << tr("通播") << tr("电台");

    chnlCfgFileName = QString("/home/root/seat_imx/conf/chnlCfg.xml");

    chnlCfgGroup = new QGroupBox(tr("Channel Configuration"), this);
    chnlCfgGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    channel1Label = new QLabel(tr("Channel 1 : "), chnlCfgGroup);
    channel1Label->setFont(QFont("Helvetica", 12, QFont::Normal));
    channel2Label = new QLabel(tr("Channel 2 : "), chnlCfgGroup);
    channel2Label->setFont(QFont("Helvetica", 12, QFont::Normal));
    channel3Label = new QLabel(tr("Channel 3 : "), chnlCfgGroup);
    channel3Label->setFont(QFont("Helvetica", 12, QFont::Normal));
    channel4Label = new QLabel(tr("Channel 4 : "), chnlCfgGroup);
    channel4Label->setFont(QFont("Helvetica", 12, QFont::Normal));


    channel1Box = new QComboBox(chnlCfgGroup);
    channel1Box->addItem(channelStrList_zh.at(0), LeftPhoneType);
    channel1Box->addItem(channelStrList_zh.at(1), RightPhoneType);
    channel1Box->addItem(channelStrList_zh.at(2), SpeakType);
    channel1Box->addItem(channelStrList_zh.at(3), RadioType);
    channel1Box->setMinimumHeight(32);

    channel2Box = new QComboBox(chnlCfgGroup);
    channel2Box->addItem(channelStrList_zh.at(0), LeftPhoneType);
    channel2Box->addItem(channelStrList_zh.at(1), RightPhoneType);
    channel2Box->addItem(channelStrList_zh.at(2), SpeakType);
    channel2Box->addItem(channelStrList_zh.at(3), RadioType);
    channel2Box->setMinimumHeight(32);

    channel3Box = new QComboBox(chnlCfgGroup);
    channel3Box->addItem(channelStrList_zh.at(0), LeftPhoneType);
    channel3Box->addItem(channelStrList_zh.at(1), RightPhoneType);
    channel3Box->addItem(channelStrList_zh.at(2), SpeakType);
    channel3Box->addItem(channelStrList_zh.at(3), RadioType);
    channel3Box->setMinimumHeight(32);

    channel4Box = new QComboBox(chnlCfgGroup);
    channel4Box->addItem(channelStrList_zh.at(0), LeftPhoneType);
    channel4Box->addItem(channelStrList_zh.at(1), RightPhoneType);
    channel4Box->addItem(channelStrList_zh.at(2), SpeakType);
    channel4Box->addItem(channelStrList_zh.at(3), RadioType);
    channel4Box->setMinimumHeight(32);

    channelBoxList.append(channel1Box);
    channelBoxList.append(channel2Box);
    channelBoxList.append(channel3Box);
    channelBoxList.append(channel4Box);

    channel1Check = new QCheckBox(chnlCfgGroup);
    //channel1Check->setFixedSize(60, 60);
    channel2Check = new QCheckBox(chnlCfgGroup);
    //channel2Check->setFixedSize(40, 40);
    channel3Check = new QCheckBox(chnlCfgGroup);
    //channel3Check->setFixedSize(40, 40);
    channel4Check = new QCheckBox(chnlCfgGroup);
    //channel4Check->setFixedSize(40, 40);

    channelCheckList.append(channel1Check);
    channelCheckList.append(channel2Check);
    channelCheckList.append(channel3Check);
    channelCheckList.append(channel4Check);

    resetBtn = new QPushButton(tr("Factory Reset"), chnlCfgGroup);
    resetBtn->setObjectName("functionBtn_small");
    resetBtn->setFixedSize(140, 64);
    applyBtn = new QPushButton(tr("Apply"), chnlCfgGroup);
    applyBtn->setObjectName("functionBtn_small");
    applyBtn->setFixedSize(140, 64);

    QGridLayout *chnlboxLayout = new QGridLayout;
    chnlboxLayout->setMargin(20);
    chnlboxLayout->addWidget(channel1Label, 0, 0);
    chnlboxLayout->addWidget(channel1Box, 0, 1);
    chnlboxLayout->addWidget(channel1Check, 0, 2);
    chnlboxLayout->addWidget(channel2Label, 1, 0);
    chnlboxLayout->addWidget(channel2Box, 1, 1);
    chnlboxLayout->addWidget(channel2Check, 1, 2);
    chnlboxLayout->addWidget(channel3Label, 2, 0);
    chnlboxLayout->addWidget(channel3Box, 2, 1);
    chnlboxLayout->addWidget(channel3Check, 2, 2);
    chnlboxLayout->addWidget(channel4Label, 3, 0);
    chnlboxLayout->addWidget(channel4Box, 3, 1);
    chnlboxLayout->addWidget(channel4Check, 3, 2);

    QHBoxLayout *chnlBtnLayout = new QHBoxLayout;
    chnlBtnLayout->setMargin(20);
    chnlBtnLayout->addWidget(resetBtn);
    chnlBtnLayout->addWidget(applyBtn);

    QVBoxLayout *chnlCfgLayout = new QVBoxLayout;
    chnlCfgLayout->setSpacing(20);
    chnlCfgLayout->addLayout(chnlboxLayout);
    chnlCfgLayout->addLayout(chnlBtnLayout);
    chnlCfgGroup->setLayout(chnlCfgLayout);
    chnlCfgGroup->setGeometry(40, 96+30, 380, 500);

    connect(resetBtn, SIGNAL(clicked()), this, SLOT(restoreDefaultSettings()));
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyNewConfiguration()));

#if LANGUAGE_CHINESE
    chnlCfgGroup->setTitle(tr("席位通道配置"));
    channel1Label->setText(tr("通道1 : "));
    channel2Label->setText(tr("通道2 : "));
    channel3Label->setText(tr("通道3 : "));
    channel4Label->setText(tr("通道4 : "));
    resetBtn->setText(tr("恢复缺省配置"));
    applyBtn->setText(tr("应用"));
#endif

    readChnlCfgFile(chnlCfgFileName);



    /* RTC Group */

    rtcGroup = new QGroupBox(tr("RTC"), this);
    rtcGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    rtcName = new QLabel(tr("Device : None"), rtcGroup);
    rtcName->setFont(QFont("Courier", 10, QFont::Normal));
    rtcDateTime = new QLabel(tr("RTC : None"), rtcGroup);
    rtcDateTime->setFont(QFont("Courier", 10, QFont::Normal));
    sysDateTime = new QLabel(tr("Sys : None"), rtcGroup);
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

        rtcName->setText("Device : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        rtcName->setText(QString("Device : %1").arg(QString(buf)));
    }
    pclose(fstream);



    /* SATA Group */

    sdiskGroup = new QGroupBox(tr("SATA Test"), this);
    sdiskGroup->setFont(QFont("Helvetica", 12, QFont::Bold));
    sdiskGroup->setGeometry(40+380+40, 96+30+200+30, 400, 270);

    sdiskArea = new QTextEdit(sdiskGroup);
    sdiskArea->setReadOnly(true);

    readTestBtn = new QPushButton(tr("Read"), sdiskGroup);
    writeTestBtn = new QPushButton(tr("Write"), sdiskGroup);
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

bool AboutPage::readChnlCfgFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    xmlReader.setDevice(&file);

    xmlReader.readNext();
    while(!xmlReader.atEnd()) {
        if(xmlReader.isStandaloneDocument()) {
            qDebug() << tr("standalone=yes");
            //xmlReader.readNext();
        }

        if(xmlReader.isStartElement()) {
            qDebug() << tr("start element");
            if(xmlReader.name() == "channel") {
                qDebug() << tr("<channel>");
                readChannelElement();
            }
            else {
                qDebug() << tr("unknown element");
                xmlReader.readNext();
            }
        }
        else {
            qDebug() << tr("skip...");
            xmlReader.readNext();
        }
    }

    file.close();

}

void AboutPage::readChannelElement()
{
    int curChannel = 0;
    qDebug() << tr("--readChannelElement--");
    xmlReader.readNext();

    while(!xmlReader.atEnd()) {

        if(xmlReader.isEndElement()) {
            qDebug() << tr("isEndElement") << xmlReader.name();
            if(xmlReader.name() == "channel") {
                xmlReader.readNext();
                break;
            }
            xmlReader.readNext();
            continue;
        }

        if(xmlReader.isStartElement()) {

            qDebug() << xmlReader.name();
            if(xmlReader.name() == "channel0") {
                curChannel = 0;
            }
            else if(xmlReader.name() == "channel1") {
                curChannel = 1;
            }
            else if(xmlReader.name() == "channel2") {
                curChannel = 2;
            }
            else if(xmlReader.name() == "channel3") {
                curChannel = 3;
            }
            else {
                qDebug() << tr("other");
                xmlReader.readNext();
                continue;
            }

            QXmlStreamAttributes attrs = xmlReader.attributes();
            for(int i=0; i<attrs.count(); i++) {
                QXmlStreamAttribute attr = attrs.at(i);
                qDebug() << tr("\t+ ") << attr.name() << attr.value();

                if(attr.name() == "type") {
                    for(int j=0; j<ChannelCount; j++) {
                        if(attr.value() == channelStrList_en.at(j).toLower()) {
                            channelBoxList.at(curChannel)->setCurrentIndex(j);
                            break;
                        }
                    }
                }
                if(attr.name() == "enable") {
                    if(attr.value() == "0") {
                        channelCheckList.at(curChannel)->setChecked(false);
                    }
                    else if(attr.value() == "1") {
                        channelCheckList.at(curChannel)->setChecked(true);
                    }
                }
            }
            xmlReader.readNext();
        }
        else {
            xmlReader.readNext();
        }
    }
}

void AboutPage::restoreDefaultSettings()
{
    QMessageBox msgBox;
    QPushButton *noBtn = msgBox.addButton(tr("NO"), QMessageBox::ActionRole);
    QPushButton *yesBtn = msgBox.addButton(tr("YES"), QMessageBox::ActionRole);
    msgBox.setText(tr("Are you sure restore default settings ?"));
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
    msgBox.exec();
    if(msgBox.clickedButton() == noBtn) {
        return ;
    } else if(msgBox.clickedButton() == yesBtn) {

        channel1Box->setCurrentIndex(LeftPhoneType);
        channel2Box->setCurrentIndex(RightPhoneType);
        channel3Box->setCurrentIndex(SpeakType);
        channel4Box->setCurrentIndex(RadioType);

        channel1Check->setChecked(true);
        channel2Check->setChecked(true);
        channel3Check->setChecked(true);
        channel4Check->setChecked(true);

        applyNewConfiguration();
    }

}

void AboutPage::applyNewConfiguration()
{
    QMessageBox msgBox;
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));

    QFile file(chnlCfgFileName);

    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        msgBox.setText(tr("Error: Can not open file.\nApply channel configuration failed."));
    }
    else {
        xmlWriter.setDevice(&file);
        xmlWriter.setAutoFormatting(true);
        // <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
        xmlWriter.writeStartDocument(tr("1.0"), true);
        xmlWriter.writeStartElement("channel");

        xmlWriter.writeStartElement("channel0");
        //xmlWriter.writeAttribute("type", channel1Box->currentText().toLower());
        xmlWriter.writeAttribute("type", channelStrList_en.at(channel1Box->currentIndex()).toLower());
        xmlWriter.writeAttribute("enable", channel1Check->isChecked()?"1":"0");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("channel1");
        //xmlWriter.writeAttribute("type", channel2Box->currentText().toLower());
        xmlWriter.writeAttribute("type", channelStrList_en.at(channel2Box->currentIndex()).toLower());
        xmlWriter.writeAttribute("enable", channel2Check->isChecked()?"1":"0");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("channel2");
        //xmlWriter.writeAttribute("type", channel3Box->currentText().toLower());
        xmlWriter.writeAttribute("type", channelStrList_en.at(channel3Box->currentIndex()).toLower());
        xmlWriter.writeAttribute("enable", channel3Check->isChecked()?"1":"0");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("channel3");
        //xmlWriter.writeAttribute("type", channel4Box->currentText().toLower());
        xmlWriter.writeAttribute("type", channelStrList_en.at(channel4Box->currentIndex()).toLower());
        xmlWriter.writeAttribute("enable", channel4Check->isChecked()?"1":"0");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
    }

    if(file.error()) {
        msgBox.setText(tr("Error: Can not write file.\nApply channel configuration failed."));
    }
    else {
        msgBox.setText(tr("Apply channel configuration finished."));
    }

    msgBox.exec();

}

void AboutPage::on_updateTimer_timeout()
{
    char buf[128];
    memset(buf, 0, sizeof(buf));

    FILE *fstream = NULL;

    if(NULL == (fstream = popen("hwclock -u", "r"))) {

        rtcDateTime->setText("RTC : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        rtcDateTime->setText(QString("RTC : %1").arg(QString(buf)));
    }

    memset(buf, 0, sizeof(buf));

    if(NULL == (fstream = popen("date", "r"))) {

        sysDateTime->setText("Sys : None");
    }else {
        fgets(buf, sizeof(buf), fstream);
        sysDateTime->setText(QString("Sys : %1").arg(QString(buf)));
    }

    pclose(fstream);
}

void AboutPage::readTestBtnOnClicked()
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

void AboutPage::writeTestBtnOnClicked()
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

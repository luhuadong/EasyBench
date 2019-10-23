#include "seatpage.h"
#include "module/sunriset/sunriset.h"
#include <QTextCodec>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define HANDFREE_START 0xB5
#define HANDFREE_STOP  0xBA

void SeatPage::initChnlCfgUI()
{
    channelStrList_en << tr("LeftPhone") << tr("RightPhone") << tr("Speak") << tr("Radio");
    channelStrList_zh << tr("左电话") << tr("右电话") << tr("通播") << tr("电台");

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
    resetBtn->setFixedSize(140, 48);
    applyBtn = new QPushButton(tr("Apply"), chnlCfgGroup);
    applyBtn->setObjectName("functionBtn_small");
    applyBtn->setFixedSize(140, 48);

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
    //chnlBtnLayout->setMargin(20);
    chnlBtnLayout->addWidget(resetBtn);
    chnlBtnLayout->addWidget(applyBtn);

    QVBoxLayout *chnlCfgLayout = new QVBoxLayout;
    chnlCfgLayout->setMargin(20);
    //chnlCfgLayout->setSpacing(20);
    chnlCfgLayout->addLayout(chnlboxLayout);
    chnlCfgLayout->addLayout(chnlBtnLayout);
    chnlCfgGroup->setLayout(chnlCfgLayout);
    chnlCfgGroup->setGeometry(40, 96+30, 380, 320);

#if LANGUAGE_CHINESE
    chnlCfgGroup->setTitle(tr("席位通道配置"));
    channel1Label->setText(tr("通道1 : "));
    channel2Label->setText(tr("通道2 : "));
    channel3Label->setText(tr("通道3 : "));
    channel4Label->setText(tr("通道4 : "));
    resetBtn->setText(tr("恢复缺省配置"));
    applyBtn->setText(tr("应用"));
#endif
}

void SeatPage::initModeCfgUI()
{
    configGroup = new QGroupBox(tr("席位模式配置"), this);
    configGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    spBtnGroup = new QButtonGroup(this);

    QRadioButton *sp1 = new QRadioButton(tr("串口一"), configGroup);
    QRadioButton *sp2 = new QRadioButton(tr("串口二"), configGroup);
    QRadioButton *sp3 = new QRadioButton(tr("串口三"), configGroup);
    QRadioButton *sp4 = new QRadioButton(tr("串口四"), configGroup);
    spBtnGroup->addButton(sp1, 1);
    spBtnGroup->addButton(sp2, 2);
    spBtnGroup->addButton(sp3, 3);
    spBtnGroup->addButton(sp4, 4);

    QHBoxLayout *spBtnLayout = new QHBoxLayout;
    spBtnLayout->addWidget(sp1);
    spBtnLayout->addWidget(sp2);
    spBtnLayout->addWidget(sp3);
    spBtnLayout->addWidget(sp4);

    towerModeBox = new QCheckBox(tr("塔台模式"), configGroup);
    longitudeLine = new QLineEdit(configGroup);
    longitudeLine->setMinimumHeight(32);
    latitudeLine = new QLineEdit(configGroup);
    latitudeLine->setMinimumHeight(32);
    sunrisetLabel = new QLabel(tr(" "), configGroup);
    sunrisetLabel->setAlignment(Qt::AlignCenter);

    configApplyBtn = new QPushButton(tr("Apply"), configGroup);
    configApplyBtn->setObjectName("functionBtn_small");
    configApplyBtn->setFixedSize(140, 48);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->setSpacing(20);
    configLayout->setMargin(30);
    configLayout->addWidget(new QLabel(tr("通信串口 : "), configGroup));
    configLayout->addLayout(spBtnLayout);
    configLayout->addStretch();

    QHBoxLayout *lonlatLayout = new QHBoxLayout;
    lonlatLayout->addWidget(new QLabel(tr("经度 : "), configGroup));
    lonlatLayout->addWidget(longitudeLine);
    lonlatLayout->addWidget(new QLabel(tr("纬度 : "), configGroup));
    lonlatLayout->addWidget(latitudeLine);

    configLayout->addLayout(lonlatLayout);
    configLayout->addWidget(sunrisetLabel);
    configLayout->addStretch();
    QHBoxLayout *towerLayout = new QHBoxLayout;
    towerLayout->addWidget(towerModeBox);
    towerLayout->addWidget(configApplyBtn);
    configLayout->addLayout(towerLayout);
    //configLayout->addWidget(new QLabel(tr("塔台模式 : "), configGroup));
    //configLayout->addWidget(towerModeBox);
    //configLayout->addStretch();
    //configLayout->addWidget(configApplyBtn);

    configGroup->setLayout(configLayout);
    configGroup->setGeometry(40+380+40, 96+30, 400, 320);

#if LANGUAGE_CHINESE
    configApplyBtn->setText(tr("应用"));
#endif

    // 根据config.ini配置文件进行初始化
    cfgFileName = QString("/home/root/seat_imx/conf/config.ini");
    QSettings configRead(cfgFileName, QSettings::IniFormat);
    int bright = configRead.value("/BackLigth/Bright").toInt();
    int port = configRead.value("/SerialPort/Port").toInt();
    int blMode = configRead.value("/DeviceInfo/BacklightMode").toInt();
    double longitude = configRead.value("/DeviceInfo/Longitude").toDouble();
    double latitude = configRead.value("/DeviceInfo/Latitude").toDouble();

    qDebug() << QString("bright=%1, port=%2, mode=%3, longitude=%4, latitude=%5").arg(bright).arg(port).arg(blMode).arg(longitude).arg(latitude);

    if(port == 1 || port == 2 || port == 3 || port == 4) {
        spBtnGroup->button(port)->setChecked(true);
    }
    if(blMode != 0) {
        towerModeBox->setChecked(true);
    }

    longitudeLine->setText(QString::number(longitude));
    latitudeLine->setText(QString::number(latitude));
}

void SeatPage::initVideoResCfgUI()
{
    videoResGroup = new QGroupBox(tr("视频分辨率设置"), this);
    videoResGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    videoEncodeStrList << tr("h263") << tr("h264");
    videoEncodeBox = new QComboBox(videoResGroup);
    videoEncodeBox->addItem(videoEncodeStrList.at(0), H263);
    videoEncodeBox->addItem(videoEncodeStrList.at(1), H264);
    videoEncodeBox->setMinimumHeight(32);

    QString videoPayload = getVideoAttribute(QString("payload"));
    QString videoSize = getVideoAttribute(QString("size"));

    for(int i=0; i<VideoEncodeCount; i++) {
        if(videoEncodeStrList.at(i) == videoPayload) {
            videoEncodeBox->setCurrentIndex(i);
            break;
        }
    }

    resH263_StrList << tr("352,288") << tr("704,576");
    bitrateH263_StrList << tr("512000") << tr("1024000");

    resH264_StrList << tr("320,240") << tr("640,480") << tr("1024,768") << tr("1280,720");
    bitrateH264_StrList << tr("170000") << tr("512000") << tr("2048000") << tr("4500000");

    resBox = new QComboBox(videoResGroup);

    if(QString("h263") == videoPayload) {
        resBox->addItem(resH263_StrList.at(0), CIF);
        resBox->addItem(resH263_StrList.at(1), CIF4);

        for(int i=0; i<VideoH263_ResCount; i++) {
            if(resH263_StrList.at(i) == videoSize) {
                resBox->setCurrentIndex(i);
                break;
            }
        }
    }
    else if(QString("h264") == videoPayload) {
        resBox->addItem(resH264_StrList.at(0), QVGA);
        resBox->addItem(resH264_StrList.at(1), VGA);
        resBox->addItem(resH264_StrList.at(2), XGA);
        resBox->addItem(resH264_StrList.at(3), P720);

        for(int i=0; i<VideoH264_ResCount; i++) {
            if(resH264_StrList.at(i) == videoSize) {
                resBox->setCurrentIndex(i);
                break;
            }
        }
    }
    resBox->setMinimumHeight(32);

    resApplyBtn = new QPushButton(tr("Apply"), videoResGroup);
    resApplyBtn->setObjectName("functionBtn_small");
    resApplyBtn->setFixedSize(100, 48);

    QFormLayout *videoCfgLayout = new QFormLayout;
    videoCfgLayout->setMargin(20);
    videoCfgLayout->addRow(tr("编码格式 : "), videoEncodeBox);
    videoCfgLayout->addRow(tr("分辨率 : "), resBox);

    QHBoxLayout *videoLayout = new QHBoxLayout;
    videoLayout->addLayout(videoCfgLayout);
    videoLayout->addWidget(resApplyBtn);

    videoResGroup->setLayout(videoLayout);
    videoResGroup->setGeometry(40, 96+30+320+20, 380, 160);

#if LANGUAGE_CHINESE
    resApplyBtn->setText(tr("应用"));
#endif
}

void SeatPage::initHfTestCfgUI()
{
    hfTestGroup = new QGroupBox(tr("麦克风免提测试"), this);
    hfTestGroup->setFont(QFont("Helvetica", 12, QFont::Bold));

    hfApplyBtn = new QPushButton(tr("Apply"), hfTestGroup);
    hfApplyBtn->setObjectName("functionBtn_small");
    hfApplyBtn->setFixedSize(140, 48);

    QHBoxLayout *hfTestLayout = new QHBoxLayout;
    hfTestLayout->addWidget(hfApplyBtn);

    hfTestGroup->setLayout(hfTestLayout);
    hfTestGroup->setGeometry(40+380+40, 96+30+320+20, 400, 160);

#if LANGUAGE_CHINESE
    hfApplyBtn->setText(tr("开始测试"));
#endif
}

SeatPage::SeatPage(GytOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    //setTitleLabelText(tr("Seat Settings"));
    setTitleLabelText(tr("席位功能设置"));

    chnlCfgFileName = g_opt->getSeatRoot() + QString("conf/chnlCfg.xml");
    settingFileName = g_opt->getSeatRoot() + QString("conf/settings.xml");

    /* Seat Channel Configuration */

    initChnlCfgUI();
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(restoreDefaultSettings()));
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyNewConfiguration()));

    readChnlCfgFile(chnlCfgFileName);

    /* Seat Mode Configuration */

    initModeCfgUI();
    connect(longitudeLine, SIGNAL(editingFinished()), this, SLOT(calSunrisetTime()));
    connect(latitudeLine,  SIGNAL(editingFinished()), this, SLOT(calSunrisetTime()));
    connect(configApplyBtn, SIGNAL(clicked()), this, SLOT(applyModeConfiguration()));

    calSunrisetTime();

    /* video resolution */

    initVideoResCfgUI();
    connect(videoEncodeBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(videoEncodeBoxChange(QString)));
    connect(resApplyBtn, SIGNAL(clicked()), this, SLOT(applyVideoResConfig()));

    //readSettingsFile(settingFileName);

    /* Handfree test */

    g_AudioPara.buffer_size_ratio = 8;
    g_AudioPara.frame_size = 160;
    g_AudioPara.max_frame_size = 640;
    g_AudioPara.sample_rate = 8000;

    initHfTestCfgUI();
    connect(hfApplyBtn, SIGNAL(clicked()), this, SLOT(applyHfTestConfig()));

    /* TCP connection establishment */

    initTcpClient();
    tcpClient->connectToHost(QHostAddress(PCB_TCP_SERVER_IP), PCB_TCP_SERVER_PORT);

}

SeatPage::~SeatPage()
{

    tcpClient->disconnectFromHost();
    delete tcpClient;
}

bool SeatPage::openXmlFile(const QString &filePath)
{
    QFile file(filePath);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << tr("(E) Open %s failed") << filePath;
        return false;
    }
    if(!settingDoc.setContent(&file)) {
        qDebug() << tr("(E) Set content to QDomDocument failed");
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool SeatPage::readChnlCfgFile(const QString &filename)
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

void SeatPage::readChannelElement()
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

void SeatPage::restoreDefaultSettings()
{
    QMessageBox msgBox;
    QPushButton *noBtn = msgBox.addButton(tr("否"), QMessageBox::ActionRole);
    QPushButton *yesBtn = msgBox.addButton(tr("是"), QMessageBox::ActionRole);
    //msgBox.setText(tr("Are you sure restore default settings ?"));
    msgBox.setText("您确定要恢复出厂配置？");
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

void SeatPage::applyNewConfiguration()
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
        //msgBox.setText(tr("Apply channel configuration finished."));
        msgBox.setText("通道配置文件写入成功！\n路径：" + chnlCfgFileName + "\t");
    }

    msgBox.exec();

}

void SeatPage::applyModeConfiguration()
{
    QSettings configWrite(cfgFileName, QSettings::IniFormat);

    int bright = configWrite.value("/BackLigth/Bright").toInt();
    int port = spBtnGroup->checkedId();
    int blMode = 0;

    if(towerModeBox->isChecked()) {
        blMode = 1;
    }

    double longitude = longitudeLine->text().toDouble();
    double latitude = latitudeLine->text().toDouble();

    qDebug() << QString("bright=%1, port=%2, mode=%3, longitude=%4, latitude=%5").arg(bright).arg(port).arg(blMode).arg(longitude).arg(latitude);

    // Write to ini file
    configWrite.setValue("/SerialPort/Port", port);
    configWrite.setValue("/DeviceInfo/BacklightMode", blMode);
    configWrite.setValue("/DeviceInfo/Longitude", longitude);
    configWrite.setValue("/DeviceInfo/Latitude", latitude);

    configWrite.sync();

    system(" sync && pkill touchScreenCtrl ");

    QMessageBox msgBox;
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
    msgBox.setText("模式配置文件写入成功！\n路径：" + cfgFileName + "\t");
    msgBox.exec();
}


QString SeatPage::getVideoAttribute(const QString &attr)
{
    /* Open */
    if(!openXmlFile(settingFileName)) {
        qDebug() << tr("(E) Open %s failed") << settingFileName;
        return NULL;
    }

    /* Read */
    QDomElement root = settingDoc.documentElement();
    if(root.tagName() != "setting") {
        return NULL;
    }

    QDomNode node = root.firstChild();
    while(!node.isNull()) {

        QDomElement elem = node.toElement();
        if(!elem.isNull()) {

            if(elem.nodeName() == "video") {

                if(elem.hasAttribute(attr)) {
                    return elem.attribute(attr);
                }
                else return NULL;
            }
        }
        node = node.nextSibling();
    }
}

bool SeatPage::readSettingsFile(const QString &filename)
{
    qDebug() << "############# Reading settings.xml";

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
            if(xmlReader.name() == "setting") {
                qDebug() << tr("<setting>");
                readSettingsElement();
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

void SeatPage::readSettingsElement()
{
    qDebug() << tr("--readSettingsElement--");
    xmlReader.readNext();

    while(!xmlReader.atEnd()) {

        if(xmlReader.isEndElement()) {
            qDebug() << tr("isEndElement") << xmlReader.name();
            if(xmlReader.name() == "setting") {
                xmlReader.readNext();
                break;
            }
            xmlReader.readNext();
            continue;
        }

        if(xmlReader.isStartElement()) {

            qDebug() << xmlReader.name();
            if(xmlReader.name() == "video") {
                qDebug() << "XML: get video tag";
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

#if 0
                if(attr.name() == "size") {
                    for(int j=0; j<VideoResCount; j++) {
                        if(attr.value() == resStrList.at(j)) {
                            resBox->setCurrentIndex(j);
                            break;
                        }
                    }
                }
                else if(attr.name() == "payload") {
                    for(int j=0; j<VideoEncodeCount; j++) {
                        if(attr.value() == videoEncodeStrList.at(j)) {
                            videoEncodeBox->setCurrentIndex(j);
                            break;
                        }
                    }
                }
#endif
            }
            xmlReader.readNext();
        }
        else {
            xmlReader.readNext();
        }
    }
}

void SeatPage::videoEncodeBoxChange(const QString &text)
{
    resBox->clear();

    if(QString("h263") == text) {
        resBox->addItem(resH263_StrList.at(0), CIF);
        resBox->addItem(resH263_StrList.at(1), CIF4);

        resBox->setCurrentIndex(0);
    }
    else if(QString("h264") == text) {
        resBox->addItem(resH264_StrList.at(0), QVGA);
        resBox->addItem(resH264_StrList.at(1), VGA);
        resBox->addItem(resH264_StrList.at(2), XGA);
        resBox->addItem(resH264_StrList.at(3), P720);

        resBox->setCurrentIndex(0);
    }
}

void SeatPage::applyVideoResConfig()
{
    /*
     * <video
     *      rtpport="50088"
     *      jitter="30"
     *      size="640,480"
     *      maxsize="352,288"
     *      payload="h264"
     *      bitrate="170000"
     *      autoadaptbitrate="0"
     *      camera="Directshow capture: USB 视频设备"
     *      transport="gateway"
     *      requesttimeout="30"
     *      queryaddrtimeout="10"
     *      symmetricrtp="0"
     *      videotimeout="1"
     * />
    */

    /* Open */
    if(!openXmlFile(settingFileName)) {
        qDebug() << tr("(E) Open %s failed") << settingFileName;
    }

    /* Modify */
    QDomElement root = settingDoc.documentElement();
    if(root.tagName() != "setting") {
        return;
    }

    QDomNode node = root.firstChild();
    while(!node.isNull()) {

        QDomElement elem = node.toElement();
        if(!elem.isNull()) {

            if(elem.nodeName() == "video") {

                qDebug() << tr("(I) I found the node -- video");
                if(elem.hasAttribute("size")) {
                    qDebug() << tr("(I) Has attribute SIZE") << elem.attribute("size");
                }

                /* update attributes: payload, size, maxsize, bitrate */
                elem.setAttribute("payload", videoEncodeBox->currentText());
                if(QString("h263") == videoEncodeBox->currentText()) {
                    elem.setAttribute("bitrate", bitrateH263_StrList.at(resBox->currentIndex()));
                }
                else {
                    elem.setAttribute("bitrate", bitrateH264_StrList.at(resBox->currentIndex()));
                }
                elem.setAttribute("size", resBox->currentText());
                elem.setAttribute("maxsize", resBox->currentText());
            }
        }
        node = node.nextSibling();
    }

    /* Save */
    QFile file(settingFileName);
    if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << tr("(E) Open settings.xml failed");
        return ;
    }
    QTextStream ts(&file);
    ts.reset();
    ts.setCodec("utf-8");
    settingDoc.save(ts, 4, QDomNode::EncodingFromTextStream);
    file.close();

    QMessageBox msgBox;
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));

    if(file.error()) {
        msgBox.setText(tr("Error: Can not write file.\nApply video resolution failed."));
    } else {
        msgBox.setText("视频分辨率设置成功！\n路径：" + settingFileName + "\t");
    }
    msgBox.exec();

    system("sync"); // 领导要求加的
}

void SeatPage::applyHfTestConfig()
{
    if(tcpClient->state() != QAbstractSocket::ConnectedState) {
        qDebug() << tr("(E) TCP client does not connect to server");

        QMessageBox msgBox;
        msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
        msgBox.setText("与控制板握手失败，请检查是否已开启TCP服务！\t");
        msgBox.exec();

        return ;
    }

    if(hfApplyBtn->text() == tr("开始测试")) {
        system("pkill adt_aec");

        qDebug() << tr("(I) Start hand-free testing");

        system("amixer -q set 'Capture Mux' LINE_IN &");
        //initAudio();
        system("amixer -q set 'PCM' 192 &");

        tcpClientSend(HANDFREE_START);
        hfApplyBtn->setText(tr("结束测试"));
    }
    else {
        system("/home/root/tools/aec/start.sh");

        qDebug() << tr("(I) Stop hand-free testing");
        tcpClientSend(HANDFREE_STOP);
        hfApplyBtn->setText(tr("开始测试"));
    }
}

void SeatPage::tcpClientSend(char msg)
{
    qDebug() << tr("(I) GytBoxWidget::send()");

#if 1
    applyCMD cmd = {0xAA, 0x07, 0xA0, 0x00, msg, 0x00, 0xEE};

    cmd.csum = calcCheckSum((unsigned char *)&cmd, sizeof(cmd)-2);

    QByteArray bytes;
    bytes.append((char *)&cmd, sizeof(cmd));

    if(-1 == tcpClient->write(bytes)) {
        qDebug() << tr("(E) TCP socket send data failed");
        return ;
    }
    qDebug() << tr("(I) TCP socket send data succeed");

#else
    QByteArray bytes = msg.toUtf8();
    bytes.append('\n');

    qDebug() << tr("(I) Send ......");

    tcpClient->write(bytes);

    qDebug() << tr("(I) Send over");
#endif
}

void SeatPage::initTcpClient()
{
    tcpClient = new QTcpSocket(this);

    connect(tcpClient, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcpClient, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(tcpClient, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

QString SeatPage::getLocalIP()
{
    QString   hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QString   localIP  = "";
    QList<QHostAddress> addList = hostInfo.addresses();

    if(!addList.isEmpty()) {

        for(int i=0; i<addList.count(); i++) {

            QHostAddress aHost = addList.at(i);
            if(QAbstractSocket::IPv4Protocol == aHost.protocol()) {
                localIP = aHost.toString();
                break;
            }
        }
    }
    return localIP;
}

void SeatPage::onConnected()
{
    qDebug() << tr("======> TCP Connect!") << tcpClient->state();
    //tcpClientSend("Fuck you!");
}

void SeatPage::onDisconnected()
{

}

void SeatPage::onSocketStateChange(QAbstractSocket::SocketState socketState)
{

}

void SeatPage::onSocketReadyRead()
{

}

void SeatPage::calSunrisetTime()
{
    float longitude = longitudeLine->text().toFloat();
    float latitude  = latitudeLine->text().toFloat();

    time_t timer = time(NULL);
    struct tm *tblock = localtime(&timer);

    double dawn, dark;

    civil_twilight(tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday,
                   (double)longitude, (double)latitude, &dawn, &dark);

    //int timezone = get_timezone();
    int timezone = 8;

    int dawnHour, dawnMin;
    timeconvert(dawn, timezone, &dawnHour, &dawnMin);

    int darkHour, darkMin;
    timeconvert(dark, timezone, &darkHour, &darkMin);

    char buf[256];
    sprintf(buf, "参考日出时间 %02d:%02d , 参考日落时间 %02d:%02d", (dawnHour)%24, dawnMin, (darkHour)%24, darkMin);

    qDebug() << tr("Timezone") << timezone << buf;

    sunrisetLabel->setText(QString(buf));
    sunrisetLabel->setStyleSheet("color: #ff0000");
}

#if 0
bool SeatPage::initAudio()
{
    unsigned int exact_rate = g_AudioPara.sample_rate;
    int period_size = g_AudioPara.frame_size;
    int buffer_size = period_size * g_AudioPara.buffer_size_ratio;

    char *captureDev = strdup("default");
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;

    int error;

    snd_pcm_hw_params_alloca(&hwparams);

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, captureDev, SND_PCM_STREAM_CAPTURE, 0) < 0)
    {
        qDebug("Error opening PCM device %s\n", captureDev);
        return(false);
    }

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0)
    {
        qDebug("Can not configure this PCM device.\n");
        return(false);
    }

    /* Set access type. */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        qDebug("Error setting access.\n");
        return(false);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
        qDebug("Error setting format.\n");
        return(false);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
        qDebug("Error setting rate.\n");
        return(false);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        qDebug("Error setting channels.\n");
        return(false);
    }

    /* Set period size. */
    if (snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, (snd_pcm_uframes_t*)&period_size, 0) < 0) {
        qDebug("Error setting periods.\n");
        return(false);
    }
    qDebug("Period size set to %d\n", period_size);

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    if (error=snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,  (snd_pcm_uframes_t*)&buffer_size) < 0)
    {
        qDebug("Error setting buffersize. %s\n", snd_strerror (error));
        return(false);
    }
    qDebug("capture Buffer size set to %d\n", buffer_size);

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0)
    {
        qDebug("Error setting HW params.\n");
        return(false);
    }

    //g_AECState.audioInfo.pcm_handle_capture = pcm_handle;

    snd_pcm_sw_params_malloc(&swparams);
    snd_pcm_sw_params_current(pcm_handle, swparams);
    snd_pcm_uframes_t val;
    snd_pcm_sw_params_get_start_threshold(swparams, &val);

    return true;
}


void frameProcess(void *ptr )
{
    bool restartOutput=false;
    short int RxOut[g_AudioPara.frame_size];  //not use it indeed
    int readerror, writeerror;
    int i,retVal;
    int j = 0;

    ADT_Int16 echo[g_AudioPara.max_frame_size];
    ADT_Int16 ref[g_AudioPara.max_frame_size];
    ADT_Int16 clean[g_AudioPara.max_frame_size];

    ADT_Int16 stereo[g_AudioPara.max_frame_size * 2];
    ADT_Int16 out_stereo[g_AudioPara.max_frame_size * 2];
    snd_pcm_uframes_t size = g_AudioPara.frame_size;


    while(1)
    {
        //qDebug("frameProcess begin");

        /* In order to support the function that use bypass mode during test for Yinliuyi
         * So
         */
        //while(g_AECState.is_enable || g_AECState.by_pass_mode)
        while(g_AECState.is_enable)
        //while(1)
        {
            readerror = snd_pcm_readi(g_AECState.audioInfo.pcm_handle_capture, stereo, size);
            if (readerror < 0)
            {
                qDebug("alsa read error (%s)\n", snd_strerror (readerror));
                if (xrunRecovery(g_AECState.audioInfo.pcm_handle_capture, readerror) < 0) {
                    qDebug("microphone: Write error: %s\n", snd_strerror(readerror));
                }
                memset(out_stereo, 0, sizeof(out_stereo));
                continue;
            }
            else
            {
                if(readerror != size)
                    qDebug("Short on samples captured: %d\n", readerror);

                for(i=0,j=0;i<size*2;i=i+2)
                {

                    //fwrite(stereo+i, sizeof(ADT_Int16), 1, g_AECState.echofile);       //left
                    //fwrite(stereo+1+i, sizeof(ADT_Int16), 1, g_AECState.reffile);       //right

                    memcpy(ref+j,stereo+i,sizeof(ADT_Int16));
                    memcpy(echo+j,stereo+1+i,sizeof(ADT_Int16));
                    j++;
                }

                if(g_AECState.is_enable == false)
                    break;


                if(g_AECState.by_pass_mode)
                {
                    for(i=0,j=0;j<size;i=i+2)
                    {
                        memcpy(out_stereo+i,echo+j,sizeof(ADT_Int16));
                        memset(out_stereo+1+i, 0, sizeof(ADT_Int16));
                        j++;
                    }

                    while ((writeerror = snd_pcm_writei (g_AECState.audioInfo.pcm_handle_playback, out_stereo, size)) < 0)
                    {
                        if (writeerror == -EAGAIN)
                            continue;
                        qDebug("alsa write error (%s)\n", snd_strerror (writeerror));
                        xrunRecovery(g_AECState.audioInfo.pcm_handle_playback, writeerror);
                        restartOutput=true;
                        break;
                    }
                    if(writeerror >=0 &&(writeerror!= size))
                        qDebug("Short on samples played: %d\n", writeerror);
                }
            }
        } // while(g_AECState.is_enable)

        usleep(2*1000);

    } // while(1)
}
#endif

#include "seatpage.h"
#include <QTextCodec>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

SeatPage::SeatPage(QWidget *parent) :
    PageWidget(parent)
{
    //setTitleLabelText(tr("Seat Settings"));
    setTitleLabelText(tr("席位功能设置"));

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

    readChnlCfgFile(chnlCfgFileName);



    /* Config Group */

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

    configApplyBtn = new QPushButton(tr("Apply"), configGroup);
    configApplyBtn->setObjectName("functionBtn_small");
    configApplyBtn->setFixedSize(140, 48);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->setSpacing(20);
    configLayout->setMargin(30);
    configLayout->addWidget(new QLabel(tr("通信串口 : "), configGroup));
    configLayout->addLayout(spBtnLayout);
    configLayout->addStretch();
    configLayout->addWidget(new QLabel(tr("当前经度 : "), configGroup));
    configLayout->addWidget(longitudeLine);
    configLayout->addWidget(new QLabel(tr("当前纬度 : "), configGroup));
    configLayout->addWidget(latitudeLine);
    configLayout->addStretch();
    configLayout->addWidget(new QLabel(tr("塔台模式 : "), configGroup));
    configLayout->addWidget(towerModeBox);
    configLayout->addStretch();
    configLayout->addWidget(configApplyBtn);

    configGroup->setLayout(configLayout);
    configGroup->setGeometry(40+380+40, 96+30, 400, 500);

    connect(configApplyBtn, SIGNAL(clicked()), this, SLOT(applyModeConfiguration()));


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

#if LANGUAGE_CHINESE
    chnlCfgGroup->setTitle(tr("席位通道配置"));
    channel1Label->setText(tr("通道1 : "));
    channel2Label->setText(tr("通道2 : "));
    channel3Label->setText(tr("通道3 : "));
    channel4Label->setText(tr("通道4 : "));
    resetBtn->setText(tr("恢复缺省配置"));
    applyBtn->setText(tr("应用"));
    configApplyBtn->setText(tr("应用"));
#endif


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


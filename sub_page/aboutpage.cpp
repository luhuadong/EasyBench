#include "aboutpage.h"
#include <QTextCodec>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

AboutPage::AboutPage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Seat Settings"));

    channelStrList << tr("LeftPhone") << tr("RightPhone") << tr("Speak") << tr("Radio");
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
    channel1Box->addItem(channelStrList.at(0), LeftPhoneType);
    channel1Box->addItem(channelStrList.at(1), RightPhoneType);
    channel1Box->addItem(channelStrList.at(2), SpeakType);
    channel1Box->addItem(channelStrList.at(3), RadioType);
    channel1Box->setMinimumHeight(32);

    channel2Box = new QComboBox(chnlCfgGroup);
    channel2Box->addItem(channelStrList.at(0), LeftPhoneType);
    channel2Box->addItem(channelStrList.at(1), RightPhoneType);
    channel2Box->addItem(channelStrList.at(2), SpeakType);
    channel2Box->addItem(channelStrList.at(3), RadioType);
    channel2Box->setMinimumHeight(32);

    channel3Box = new QComboBox(chnlCfgGroup);
    channel3Box->addItem(channelStrList.at(0), LeftPhoneType);
    channel3Box->addItem(channelStrList.at(1), RightPhoneType);
    channel3Box->addItem(channelStrList.at(2), SpeakType);
    channel3Box->addItem(channelStrList.at(3), RadioType);
    channel3Box->setMinimumHeight(32);

    channel4Box = new QComboBox(chnlCfgGroup);
    channel4Box->addItem(channelStrList.at(0), LeftPhoneType);
    channel4Box->addItem(channelStrList.at(1), RightPhoneType);
    channel4Box->addItem(channelStrList.at(2), SpeakType);
    channel4Box->addItem(channelStrList.at(3), RadioType);
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
                        if(attr.value() == channelStrList.at(j).toLower()) {
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

        channel1Check->setEnabled(true);
        channel2Check->setEnabled(true);
        channel3Check->setEnabled(true);
        channel4Check->setEnabled(true);

        applyNewConfiguration();
    }

}

void AboutPage::applyNewConfiguration()
{
    QFile file(QString("test.xml"));
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    xmlWriter.setDevice(&file);
    xmlWriter.setAutoFormatting(true);
    // <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    xmlWriter.writeStartDocument(tr("1.0"), true);
    xmlWriter.writeStartElement("channel");
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();

    QMessageBox msgBox;
    msgBox.setText(tr("Apply channel configuration finished."));
    msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
    msgBox.exec();

}

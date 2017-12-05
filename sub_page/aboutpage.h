#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include "custom_widget/pagewidget.h"

#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QXmlStreamAttribute>
#include <QPushButton>

class AboutPage : public PageWidget
{
    Q_OBJECT
public:
    explicit AboutPage(QWidget *parent = 0);

public slots:

private slots:
    void restoreDefaultSettings();
    void applyNewConfiguration();

private:
    bool readChnlCfgFile(const QString &filename);
    void readChannelElement();

    QStringList channelStrList;
    QString chnlCfgFileName;

    QXmlStreamReader xmlReader;
    QXmlStreamWriter xmlWriter;

    typedef enum ChannelType
    {
        LeftPhoneType  = 0,
        RightPhoneType = 1,
        SpeakType      = 2,
        RadioType      = 3,
        ChannelCount   = 4

    } ChannelType;

    QGroupBox *chnlCfgGroup;
    QLabel *channel1Label;
    QLabel *channel2Label;
    QLabel *channel3Label;
    QLabel *channel4Label;
    QList<QComboBox *> channelBoxList;
    QComboBox *channel1Box;
    QComboBox *channel2Box;
    QComboBox *channel3Box;
    QComboBox *channel4Box;
    QList<QCheckBox *> channelCheckList;
    QCheckBox *channel1Check;
    QCheckBox *channel2Check;
    QCheckBox *channel3Check;
    QCheckBox *channel4Check;
    QPushButton *resetBtn;
    QPushButton *applyBtn;


};

#endif // ABOUTPAGE_H

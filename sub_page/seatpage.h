#ifndef SEATPAGE_H
#define SEATPAGE_H

#include "custom_widget/pagewidget.h"
#include "gyt_common.h"

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
#include <QTimer>
#include <QTextEdit>

class SeatPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SeatPage(QWidget *parent = 0);

public slots:

private slots:
    void restoreDefaultSettings();
    void applyNewConfiguration();

    void on_updateTimer_timeout(void);
    void readTestBtnOnClicked(void);
    void writeTestBtnOnClicked(void);

private:
    bool readChnlCfgFile(const QString &filename);
    void readChannelElement();

    QStringList channelStrList_zh;
    QStringList channelStrList_en;
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

    QTimer *updateTimer;

    QGroupBox *rtcGroup;
    QLabel *rtcName;
    QLabel *rtcDateTime;
    QLabel *sysDateTime;

    QGroupBox *sdiskGroup; // SATA interface disk
    QTextEdit *sdiskArea;
    QPushButton *readTestBtn;
    QPushButton *writeTestBtn;

};

#endif // SEATPAGE_H

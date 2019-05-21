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
#include <QDomDocument>
#include <QDomElement>
#include <QPushButton>
#include <QTextEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QSettings>
#include <QTcpSocket>
#include <QHostInfo>

class SeatPage : public PageWidget
{
    Q_OBJECT
public:
    explicit SeatPage(QWidget *parent = 0);
    ~SeatPage();

    void tcpClientSend(char msg);

public slots:

private slots:
    void restoreDefaultSettings();
    void applyNewConfiguration();

    void calSunrisetTime();

    void applyModeConfiguration();
    void applyVideoResConfig();
    void applyHfTestConfig();

    void onConnected();
    void onDisconnected();
    void onSocketStateChange(QAbstractSocket::SocketState socketState);
    void onSocketReadyRead(); // Read data from socket

private:
    bool openXmlFile(const QString &filePath);

    /* chnlCfg.xml */
    bool readChnlCfgFile(const QString &filename);
    void readChannelElement();

    bool readSettingsFile(const QString &filename);
    void readSettingsElement();

    void initChnlCfgUI();
    void initModeCfgUI();
    void initVideoResCfgUI();
    void initHfTestCfgUI();

    //bool initAudio();

    /* Socket */

    void initTcpClient();
    QString getLocalIP();

    QTcpSocket *tcpClient;

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

    /* config.ini */
    QGroupBox *configGroup;

    QButtonGroup *spBtnGroup;
    QCheckBox *towerModeBox;
    QLineEdit *longitudeLine;
    QLineEdit *latitudeLine;
    QLabel *sunrisetLabel;
    QPushButton *configApplyBtn;

    QString cfgFileName;

    /* video resolution */
    typedef enum ResType
    {
        QVGA = 0,
        VGA  = 1,
        XGA  = 2,
        P720 = 3,
        ResCount = 4

    } ResType;

    QString settingFileName;
    QDomDocument settingDoc;

    QGroupBox *videoResGroup;
    QStringList resStrList;
    QStringList bitrateStrList;
    QComboBox *resBox;
    QPushButton *resApplyBtn;

    /* Handfree test */
    QGroupBox *hfTestGroup;
    QPushButton *hfApplyBtn;
    Audio_Parameter g_AudioPara;


};

#endif // SEATPAGE_H

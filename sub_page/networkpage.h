#ifndef NETWORKPAGE_H
#define NETWORKPAGE_H

#include "custom_widget/pagewidget.h"

#include "module/network/eb_net_util.h"

#include <QComboBox>
#include <QFile>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>

class NetworkPage : public PageWidget
{
    Q_OBJECT
public:
    explicit NetworkPage(EbOptions *options, QWidget *parent = nullptr);

    QString defaultStatusHint() const override;

    void toggleInterfaceLink();
    void syncLinkToggleButton();

private slots:
    void refreshInterfaces();
    void onInterfaceChanged(int index);
    void applyMacRuntime();
    void saveMacPersistent();
    void applyIpv4Runtime();
    void saveIpv4Persistent();
    void setInterfaceUp();
    void setInterfaceDown();
    void startPing();
    void stopPing();
    void onPingLine(const QString &line);
    void onPingFinished(int exitCode);

    void i210EditClicked();
    void i210CheckClicked();
    void i210DumpClicked();
    void i210WriteClicked();

private:
    void buildUi();
    void buildI210Tab(QWidget *tabPage);
    bool generateI210File();
    QString parseI210MacLine();
    void appendMacLog(const QString &text);
    void appendIpv4Log(const QString &text);
    void appendPingLog(const QString &text);
    void applyPrivilegeUi();
    bool confirmNetworkChange(const QString &title, const QString &detail);
    QString currentInterfaceName() const;
    void updateStatusLabels();
    void fillConfigFields(const EbNet::InterfaceInfo &info);
    bool checkPrivilegeForApply();

    EbNet::PingRunner *pingRunner = nullptr;
    QTabWidget *tabWidget = nullptr;
    int i210TabIndex = -1;

    QComboBox *ifaceBox = nullptr;
    QPushButton *linkToggleBtn = nullptr;

    QLabel *statusMacLabel = nullptr;
    QLabel *statusIpLabel = nullptr;
    QLabel *statusGwLabel = nullptr;
    QLabel *statusDnsLabel = nullptr;
    QLabel *statusLinkLabel = nullptr;
    QLabel *statusBackendLabel = nullptr;

    QLineEdit *macEdit = nullptr;
    QPushButton *macApplyBtn = nullptr;
    QPushButton *macSaveBtn = nullptr;
    QTextEdit *macLogArea = nullptr;

    QRadioButton *dhcpRadio = nullptr;
    QRadioButton *staticRadio = nullptr;
    QLineEdit *ipEdit = nullptr;
    QLineEdit *maskEdit = nullptr;
    QLineEdit *gatewayEdit = nullptr;
    QLineEdit *dnsEdit = nullptr;
    QSpinBox *prefixSpin = nullptr;
    QPushButton *ipApplyBtn = nullptr;
    QPushButton *ipSaveBtn = nullptr;
    QTextEdit *ipv4LogArea = nullptr;

    QLineEdit *pingHostEdit = nullptr;
    QSpinBox *pingCountSpin = nullptr;
    QPushButton *pingBtn = nullptr;
    QPushButton *pingStopBtn = nullptr;
    QLabel *pingSummaryLabel = nullptr;
    QTextEdit *pingLogArea = nullptr;

    QLineEdit *i210MacLine = nullptr;
    QTextEdit *i210LogArea = nullptr;

    QList<EbNet::InterfaceInfo> interfaces;

    char i210MacBytes[6] = {};
    QStringList i210MacParts;
    QFile i210TmpFile;
};

#endif /* NETWORKPAGE_H */

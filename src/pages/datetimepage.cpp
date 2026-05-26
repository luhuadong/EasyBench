#include "datetimepage.h"
#include "widgets/tb_widget_util.h"

#include <QTextCodec>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExp>
#include <QValidator>
#include <QIntValidator>
#include <QRegExpValidator>

DatetimePage::DatetimePage(TbOptions *options, QWidget *parent) :
    PageWidget(options, parent)
{
    setTitleLabelText(tr("Setting panel"));

    networkGroupBox = new QGroupBox(tr("  Network  "), this);
    ipLineEdit = new InputLineEdit(networkGroupBox);
    maskLineEdit = new InputLineEdit(networkGroupBox);
    gatewayLineEdit = new InputLineEdit(networkGroupBox);
    dnsLineEdit = new InputLineEdit(networkGroupBox);
    //dnsLineEdit->setReadOnly(true);

    QRegExp ipRx("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){4}");
    /*
    QRegExp ipRx("^([0-1][0-9]{2}|[2][0-5]{2})[.]"
                               "([0-1][0-9]{2}|[2][0-5]{2})[.]"
                               "([0-1][0-9]{2}|[2][0-5]{2})[.]"
                               "([0-1][0-9]{2}|[2][0-5]{2})$");
                               */

    QRegExpValidator *ipValidator = new QRegExpValidator(ipRx, this);
    ipLineEdit->setValidator(ipValidator);
    maskLineEdit->setValidator(ipValidator);
    gatewayLineEdit->setValidator(ipValidator);
    dnsLineEdit->setValidator(ipValidator);
    //ipLineEdit->setInputMask(QString("000.000.000.000"));
    maskLineEdit->setInputMask(QString("000.000.000.000; "));
    gatewayLineEdit->setInputMask(QString("000.000.000.000;0"));
    dnsLineEdit->setInputMask(QString("000.000.000.000"));

    datetimeGroupBox = new QGroupBox(tr("  Time-zone & Date-time  "), this);
    timezoneComboBox = new QComboBox(datetimeGroupBox);
    dateLineEdit = new InputLineEdit(datetimeGroupBox);
    timeLineEdit = new InputLineEdit(datetimeGroupBox);

    dateLineEdit->setInputMask(QString("0000-00-00"));
    timeLineEdit->setInputMask(QString("00:00:00"));

    TbWidget::FormGridBuilder networkGrid(networkGroupBox);
    networkGrid.addLabeledRow(tr("IP : "), ipLineEdit);
    networkGrid.addLabeledRow(tr("Mask : "), maskLineEdit);
    networkGrid.addLabeledRow(tr("Gateway : "), gatewayLineEdit);
    networkGrid.addLabeledRow(tr("DNS : "), dnsLineEdit);

    TbWidget::FormGridBuilder datetimeGrid(datetimeGroupBox);
    datetimeGrid.addLabeledRow(tr("Time-zone : "), timezoneComboBox);
    datetimeGrid.addLabeledRow(tr("Date : "), dateLineEdit);
    datetimeGrid.addLabeledRow(tr("Time : "), timeLineEdit);
    TbWidget::applyComboBoxStyle(timezoneComboBox);
    TbWidget::applyLineEditStyle(ipLineEdit);
    TbWidget::applyLineEditStyle(maskLineEdit);
    TbWidget::applyLineEditStyle(gatewayLineEdit);
    TbWidget::applyLineEditStyle(dnsLineEdit);
    TbWidget::applyLineEditStyle(dateLineEdit);
    TbWidget::applyLineEditStyle(timeLineEdit);

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setGeometry(0, 96, this->width(), this->height()-96-72);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    mainLayout->addWidget(networkGroupBox);
    mainLayout->addWidget(datetimeGroupBox);

    mainWidget->setLayout(mainLayout);
}

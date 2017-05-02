#include "datetimepage.h"
#include <QTextCodec>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QRegExp>
#include <QValidator>
#include <QIntValidator>
#include <QRegExpValidator>

DatetimePage::DatetimePage(QWidget *parent) :
    PageWidget(parent)
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

    QFormLayout *leftLayout = new QFormLayout;
    //leftLayout->setLabelAlignment(Qt::AlignRight);
    //leftLayout->setContentsMargins(10, 20, 10, 300);
    leftLayout->setMargin(20);
    leftLayout->setSpacing(10);
    leftLayout->addRow(tr("IP : "), ipLineEdit);
    leftLayout->addRow(tr("Mask : "), maskLineEdit);
    leftLayout->addRow(tr("Gateway : "), gatewayLineEdit);
    leftLayout->addRow(tr("DNS : "), dnsLineEdit);
    networkGroupBox->setLayout(leftLayout);


    datetimeGroupBox = new QGroupBox(tr("  Time-zone & Date-time  "), this);
    timezoneComboBox = new QComboBox(datetimeGroupBox);
    dateLineEdit = new InputLineEdit(datetimeGroupBox);
    timeLineEdit = new InputLineEdit(datetimeGroupBox);

    dateLineEdit->setInputMask(QString("0000-00-00"));
    timeLineEdit->setInputMask(QString("00:00:00"));

    QFormLayout *rightLayout = new QFormLayout;
    //rightLayout->setContentsMargins(10, 20, 10, 300);
    rightLayout->setMargin(20);
    rightLayout->setSpacing(10);
    rightLayout->addRow(tr("Time-zone : "), timezoneComboBox);
    rightLayout->addRow(tr("Date : "), dateLineEdit);
    rightLayout->addRow(tr("Time : "), timeLineEdit);
    datetimeGroupBox->setLayout(rightLayout);

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setGeometry(0, 96, this->width(), this->height()-96-72);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    mainLayout->addWidget(networkGroupBox);
    mainLayout->addWidget(datetimeGroupBox);

    mainWidget->setLayout(mainLayout);
}

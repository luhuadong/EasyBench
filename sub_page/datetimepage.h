#ifndef DATETIMEPAGE_H
#define DATETIMEPAGE_H

#include "custom_widget/pagewidget.h"
#include "custom_widget/inputlineedit.h"

#include <QString>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>


class DatetimePage : public PageWidget
{
    Q_OBJECT
public:
    explicit DatetimePage(GytOptions *options, QWidget *parent = 0);

public slots:

private:

    QGroupBox *networkGroupBox;
    InputLineEdit *ipLineEdit;
    InputLineEdit *maskLineEdit;
    InputLineEdit *gatewayLineEdit;
    InputLineEdit *dnsLineEdit;

    QGroupBox *datetimeGroupBox;
    QComboBox *timezoneComboBox;
    InputLineEdit *dateLineEdit;
    InputLineEdit *timeLineEdit;


};

#endif // DATETIMEPAGE_H

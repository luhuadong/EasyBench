#ifndef REALTIMEPAGE_H
#define REALTIMEPAGE_H

#include "custom_widget/pagewidget.h"

#include <QString>

class RealtimePage : public PageWidget
{
    Q_OBJECT
public:
    explicit RealtimePage(QWidget *parent = 0);

public slots:

private:
    //QTextEdit *textEdit;    // 显示天气

    QString city;//用于获取地方的字符串
    QString weather;//用于获取天气的字符串

    QLabel *tempLabel;
    QLabel *humiLabel;

};

#endif // REALTIMEPAGE_H

#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include "custom_widget/pagewidget.h"

#include <QString>

class AboutPage : public PageWidget
{
    Q_OBJECT
public:
    explicit AboutPage(QWidget *parent = 0);

public slots:

private:
    //QTextEdit *textEdit;    // 显示天气

    QString city;//用于获取地方的字符串
    QString weather;//用于获取天气的字符串

    QLabel *tempLabel;
    QLabel *humiLabel;

};

#endif // ABOUTPAGE_H

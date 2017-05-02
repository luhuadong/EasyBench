#ifndef TOUCHPAGE_H
#define TOUCHPAGE_H

#include "custom_widget/pagewidget.h"

#include <QString>
#include <QPushButton>

class TouchPage : public PageWidget
{
    Q_OBJECT
public:
    explicit TouchPage(QWidget *parent = 0);

public slots:

private:
    QPushButton *calBtn;
    QPushButton *linz9Btn;
    QPushButton *linzBtn;
    QPushButton *drawBtn;

private slots:
    void calBtnClicked();
    void linz9BtnClicked();
    void linzBtnClicked();
    void drawBtnClicked();

};

#endif // TOUCHPAGE_H

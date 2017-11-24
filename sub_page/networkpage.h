#ifndef NETWORKPAGE_H
#define NETWORKPAGE_H

#include "custom_widget/pagewidget.h"

#include <QNetworkInterface>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QString>
#include <QStringList>
#include <QFile>

class NetworkPage : public PageWidget
{
    Q_OBJECT
public:
    explicit NetworkPage(QWidget *parent = 0);

signals:

public slots:

private slots:
    void editBtnOnClicked(void);
    void checkBtnOnClicked(void);
    void dumpBtnOnClicked(void);
    void writeBtnOnClicked(void);

private:
    QList<QNetworkInterface> ifList;

    bool hasI210OnBoard();
    QString getMacAddrFromEditLine();
    bool generateI210File();

    QGroupBox *i210Group;
    QLabel *macAddrLabel;
    QLineEdit *macAddrLine;
    QPushButton *editBtn;
    QPushButton *checkBtn;
    QPushButton *dumpBtn;
    QPushButton *writeBtn;
    QTextEdit *showArea;

    char MACAddr[6];
    QString MACAddrStr;
    QStringList MACAddrList;

    QFile tmpFile;

};

#endif // NETWORKPAGE_H

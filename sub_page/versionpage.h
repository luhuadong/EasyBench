#ifndef VERSIONPAGE_H
#define VERSIONPAGE_H

#include "custom_widget/pagewidget.h"

#include <QString>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QSettings>

class VersionPage : public PageWidget
{
    Q_OBJECT
public:
    explicit VersionPage(GytOptions *options, QWidget *parent = 0);

public slots:

private slots:
    void shutdownSystem();
    void rebootSystem();

private:
    QTableView *tabView;
    QStandardItemModel *tabModel;

    QString gyVersionFile;

    void getVersionData(void);

};

#endif // VERSIONPAGE_H

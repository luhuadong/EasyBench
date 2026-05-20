#ifndef VERSIONPAGE_H
#define VERSIONPAGE_H

#include "widgets/pagewidget.h"

#include <QString>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
class VersionPage : public PageWidget
{
    Q_OBJECT
public:
    explicit VersionPage(EbOptions *options, QWidget *parent = 0);

private:
    QTableView *tabView;
    QStandardItemModel *tabModel;

    void getVersionData(void);
};

#endif // VERSIONPAGE_H

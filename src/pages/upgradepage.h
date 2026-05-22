#ifndef UPGRADEPAGE_H
#define UPGRADEPAGE_H

#include "widgets/pagewidget.h"
#include "modules/update/tb_update_util.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

class UpgradePage : public PageWidget
{
    Q_OBJECT
public:
    explicit UpgradePage(TbOptions *options, QWidget *parent = nullptr);

    bool isUpgradeBusy() const { return upgradeBusy; }
    bool canStartUpgrade() const;

    QString defaultStatusHint() const override;

public slots:
    void selectPackage();
    void verifyPackage();
    void startUpgrade();
    void shutdownSystem();
    void rebootSystem();

private slots:
    void onUpdateLine(const QString &line);
    void onUpdateFinished(bool success, int exitCode);

private:
    void buildUi();
    void appendLog(const QString &text);
    void refreshInfoLabels(const TbUpdate::PackageInfo &info);
    bool confirmUpgrade();
    void setUpgradeBusy(bool busy);
    void syncActionButtons();

    TbUpdate::UpdateRunner *updateRunner = nullptr;
    TbUpdate::PackageInfo lastInfo;
    bool upgradeBusy = false;

    QPushButton *selectBtn = nullptr;
    QPushButton *verifyBtn = nullptr;
    QPushButton *startBtn = nullptr;
    QPushButton *shutdownBtn = nullptr;
    QPushButton *rebootBtn = nullptr;

    QLineEdit *packagePathEdit = nullptr;
    QLabel *infoPathLabel = nullptr;
    QLabel *infoSizeLabel = nullptr;
    QLabel *infoVersionLabel = nullptr;
    QLabel *infoProductLabel = nullptr;
    QLabel *infoDescLabel = nullptr;
    QLabel *infoStatusLabel = nullptr;
    QTextEdit *logArea = nullptr;
};

#endif /* UPGRADEPAGE_H */

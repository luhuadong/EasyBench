#ifndef STORAGEPAGE_H
#define STORAGEPAGE_H

#include "widgets/pagewidget.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QElapsedTimer>
#include <QThread>

class StorageTestWorker : public QThread
{
    Q_OBJECT
public:
    enum class TestKind {
        MemoryStress = 0,
        DiskRead,
        DiskWrite
    };

    StorageTestWorker(TestKind kind,
                      const QString &targetPath,
                      int sizeMb,
                      int loops,
                      QObject *parent = nullptr);

signals:
    void logLine(const QString &line);
    void progressChanged(int percent);
    void finished(bool ok, const QString &summary);

protected:
    void run() override;

private:
    bool runMemoryTest();
    bool runDiskRead();
    bool runDiskWrite();
    void emitTestReport(bool ok);

    TestKind m_kind;
    QString m_targetPath;
    int m_sizeMb;
    int m_loops;
    QElapsedTimer m_timer;
    double m_lastDdMbps = -1.0;
};

class StoragePage : public PageWidget
{
    Q_OBJECT
public:
    explicit StoragePage(EbOptions *options, QWidget *parent = nullptr);
    ~StoragePage();

private slots:
    void refreshTargets();
    void startTest();
    void stopTest();
    void onLogLine(const QString &line);
    void onProgress(int percent);
    void onTestFinished(bool ok, const QString &summary);

private:
    void buildUi();
    void setUiBusy(bool busy);
    void appendLog(const QString &line);

    StorageTestWorker *worker = nullptr;

    QComboBox *mediaBox = nullptr;
    QComboBox *testTypeBox = nullptr;
    QSpinBox *sizeSpin = nullptr;
    QSpinBox *loopsSpin = nullptr;
    QPushButton *startBtn = nullptr;
    QPushButton *stopBtn = nullptr;
    QProgressBar *progressBar = nullptr;
    QTextEdit *logArea = nullptr;
    QLabel *statusLabel = nullptr;
};

#endif /* STORAGEPAGE_H */

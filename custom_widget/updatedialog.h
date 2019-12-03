#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QDir>

class UpdateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

private slots:
    void selectFile();
    void updateFile();
    void closeDialog();

private:
    QString target;

    QLabel *fileLabel;
    QLineEdit *fileLineEdit;
    QPushButton *fileBtn;

    QPushButton *updateBtn;
    QPushButton *cancelBtn;
};

#endif // UPDATEDIALOG_H

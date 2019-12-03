#include "updatedialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("升级席位界面"));
    setFixedSize(400, 300);


    fileLabel = new QLabel(tr("目标文件："), this);
    fileLineEdit = new QLineEdit("/run/media/sda1/seat_imx.tar.gz", this);
    fileLineEdit->setObjectName("inputLineEdit");
    fileLineEdit->setReadOnly(true);
    fileBtn = new QPushButton(tr("选择"), this);
    fileBtn->setFixedSize(40, 40);
    fileBtn->setObjectName("functionBtn_small");

    target = fileLineEdit->text();

    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(fileLineEdit);
    fileLayout->addWidget(fileBtn);

    updateBtn = new QPushButton(tr("升级"), this);
    updateBtn->setFixedSize(80, 40);
    updateBtn->setObjectName("functionBtn_small");
    cancelBtn = new QPushButton(tr("取消"), this);
    cancelBtn->setFixedSize(80, 40);
    cancelBtn->setObjectName("functionBtn_small");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(updateBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(fileLayout);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(fileBtn, SIGNAL(clicked(bool)), this, SLOT(selectFile()));
    connect(updateBtn, SIGNAL(clicked(bool)), this, SLOT(updateFile()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(closeDialog()));
}

UpdateDialog::~UpdateDialog()
{

}

void UpdateDialog::selectFile()
{
    target = QFileDialog::getOpenFileName(this, "选择升级文件", "/", "*.tar.gz");
    qDebug() << target;
    fileLineEdit->setText(target);
}

void UpdateDialog::updateFile()
{
    if(target.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
        msgBox.setText("请选择文件！");
        msgBox.exec();
        return ;
    }

    QFile file;
    file.setFileName(target);
    if (!file.exists()) {
        QMessageBox msgBox;
        msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
        msgBox.setText("文件不存在！");
        msgBox.exec();
        return ;
    }



        char cmd[128];
        sprintf(cmd, "tar zxf %s -C /", target.toLatin1().data());
        qDebug() << cmd;
        system(cmd);
        system("sync");

        QMessageBox msgBox;
        msgBox.setFont(QFont("Helvetica", 14, QFont::Normal));
        msgBox.setText("升级成功");
        msgBox.exec();

}

void UpdateDialog::closeDialog()
{
    this->close();
}

#include "gytboxwidget.h"
#include "gyt_common.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>

extern "C"
{
#include <stdlib.h>
}


int main(int argc, char *argv[])
{
    // Ensure only one process is running at the same time.
    QFile lockFile;
    lockFile.setFileName(QString("/home/root/.lock_gyt_box"));
    if (lockFile.exists()) {
        return 0;
    }
    else {
        system("touch /home/root/.lock_gyt_box");
    }

    QApplication a(argc, argv);

    QFile qssFile(":/qss/gyt_box.qss");
    qssFile.open(QFile::ReadOnly);
    a.setStyleSheet(qssFile.readAll());
    qssFile.close();

    calcCheckSum("1234567890", 10);
    detectDevices();

    GytBoxWidget w;
    w.show();

    return a.exec();
}

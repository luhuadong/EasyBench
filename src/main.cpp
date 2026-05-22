#include "mainwidget.h"
#include "tb_common.h"
#include "tb_options.h"
#include "tb_startup.h"
#include "tb_branding.h"
#include "tb_paths.h"
#include "tb_thread_util.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include "tb_qt_compat.h"
#include <QDebug>
#include <QFontDatabase>
#include <QFont>

extern "C"
{
#include <stdlib.h>
}


int main(int argc, char *argv[])
{
#if SINGLETON
    // Ensure only one process is running at the same time.
#if 1
    QFile lockFile;
    lockFile.setFileName(QString("~/.lock_tuxibit"));
    if (lockFile.exists()) {
        return 0;
    }
    else {
        //system("touch /home/root/.lock_tuxibit");
    }
#else
    if(!is_singleton()) {
        printf("(W) The program is already running.\n");
        return -1;
    }
#endif
#endif

    QApplication a(argc, argv);
    TbThread::setCurrentThreadName("eb-main");
    TbBranding::applyApplicationIdentity(a);

    QFile qssFile(":/qss/tuxibit.qss");
    qssFile.open(QFile::ReadOnly);
    a.setStyleSheet(qssFile.readAll());
    qssFile.close();

#if LANGUAGE_CHINESE

    tb_set_utf8_locale();

    const QString fontPath = TbPaths::chineseFontFile();
    if (fontPath.isEmpty()) {
        qWarning("TuxiBit: Chinese font not found (LiHeiPro.ttf), using system default");
    } else {
        const int index = QFontDatabase::addApplicationFont(fontPath);
        if (index == -1) {
            qWarning().noquote() << "TuxiBit: failed to load font file:" << fontPath;
        } else {
            const QStringList fontList = QFontDatabase::applicationFontFamilies(index);
            qWarning().noquote() << "TuxiBit: loaded font file:" << fontPath
                                 << "families:" << fontList.join(QLatin1String(", "));
            if (!fontList.isEmpty()) {
                QFont font_zh(fontList.at(0));
                font_zh.setBold(false);
                a.setFont(font_zh);
            } else {
                qWarning("TuxiBit: font file loaded but no families returned, using system default");
            }
        }
    }
#endif

    TbOptions g_opt;
    const TbStartupReport report = tb_check_runtime(g_opt);

    if (!report.ok()) {
        QMessageBox::critical(nullptr, QObject::tr("TuxiBit 启动失败"), report.summaryText());
        return 1;
    }

    if (!report.warnings.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(QObject::tr("TuxiBit"));
        msgBox.setText(QObject::tr("部分硬件或配置不可用，部分功能将受限。"));
        msgBox.setDetailedText(report.summaryText());
#ifdef WINDOW_DESKTOP
        msgBox.addButton(QObject::tr("继续运行"), QMessageBox::AcceptRole);
        msgBox.addButton(QObject::tr("退出"), QMessageBox::RejectRole);
        if (msgBox.exec() != 0) {
            return 1;
        }
#else
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
#endif
    }

    MainWidget w(&g_opt);
    w.show();

    return a.exec();
}

#include "eb_branding.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

namespace EbBranding {

QString applicationName()
{
    return QStringLiteral("EasyBench");
}

QString desktopFileName()
{
    return QStringLiteral("easybench");
}

static QIcon loadInstalledIcon()
{
    static const char *paths[] = {
        "/usr/share/icons/hicolor/256x256/apps/easybench.png",
        "/usr/share/icons/hicolor/128x128/apps/easybench.png",
        "/usr/share/icons/hicolor/48x48/apps/easybench.png",
        "/usr/local/share/icons/hicolor/256x256/apps/easybench.png",
        "/usr/share/pixmaps/easybench.png",
        "/usr/local/share/pixmaps/easybench.png",
    };

    for (const char *path : paths) {
        if (QFile::exists(QString::fromUtf8(path))) {
            QIcon icon(QString::fromUtf8(path));
            if (!icon.isNull()) {
                return icon;
            }
        }
    }
    return QIcon();
}

QIcon applicationIcon()
{
    const QIcon embedded(QStringLiteral(":/images/logo.png"));
    if (!embedded.isNull()) {
        return embedded;
    }
    return loadInstalledIcon();
}

void applyApplicationIdentity(QApplication &app)
{
    app.setApplicationName(applicationName());
    app.setApplicationDisplayName(applicationName());
    app.setOrganizationName(QStringLiteral("EasyBench"));
    app.setOrganizationDomain(QStringLiteral("easybench.local"));

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    app.setDesktopFileName(desktopFileName());
#endif

    const QIcon icon = applicationIcon();
    if (!icon.isNull()) {
        app.setWindowIcon(icon);
    }
}

} // namespace EbBranding

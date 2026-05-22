#include "tb_branding.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

namespace TbBranding {

QString applicationName()
{
    return QStringLiteral("TuxiBit");
}

QString desktopFileName()
{
    return QStringLiteral("tuxibit");
}

static QIcon loadInstalledIcon()
{
    static const char *paths[] = {
        "/usr/share/icons/hicolor/256x256/apps/tuxibit.png",
        "/usr/share/icons/hicolor/128x128/apps/tuxibit.png",
        "/usr/share/icons/hicolor/48x48/apps/tuxibit.png",
        "/usr/local/share/icons/hicolor/256x256/apps/tuxibit.png",
        "/usr/share/pixmaps/tuxibit.png",
        "/usr/local/share/pixmaps/tuxibit.png",
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
    /* Leave display name empty so the window title is not prefixed (e.g. "TuxiBit - …"). */
    app.setApplicationDisplayName(QString());
    app.setOrganizationName(QStringLiteral("TuxiBit"));
    app.setOrganizationDomain(QStringLiteral("tuxibit.local"));

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    app.setDesktopFileName(desktopFileName());
#endif

    const QIcon icon = applicationIcon();
    if (!icon.isNull()) {
        app.setWindowIcon(icon);
    }
}

} // namespace TbBranding

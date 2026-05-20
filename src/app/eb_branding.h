#ifndef EB_BRANDING_H
#define EB_BRANDING_H

#include <QIcon>
#include <QString>

namespace EbBranding {

QString applicationName();
QString desktopFileName();
QIcon applicationIcon();

void applyApplicationIdentity(class QApplication &app);

} // namespace EbBranding

#endif /* EB_BRANDING_H */

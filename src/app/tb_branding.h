#ifndef TB_BRANDING_H
#define TB_BRANDING_H

#include <QIcon>
#include <QString>

namespace TbBranding {

QString applicationName();
QString desktopFileName();
QIcon applicationIcon();

void applyApplicationIdentity(class QApplication &app);

} // namespace TbBranding

#endif /* TB_BRANDING_H */

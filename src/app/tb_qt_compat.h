#ifndef TB_QT_COMPAT_H
#define TB_QT_COMPAT_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  define TB_QT6 1
#else
#  define TB_QT6 0
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && !TB_QT6
#  define TB_QT5_MULTIMEDIA 1
#else
#  define TB_QT5_MULTIMEDIA 0
#endif

#if TB_QT6
#  define TB_QT6_MULTIMEDIA 1
#else
#  define TB_QT6_MULTIMEDIA 0
#endif

#if TB_QT6
#  include <QStringConverter>
inline void tb_set_utf8_locale()
{
    /* Qt 6 defaults to UTF-8 for QString and locale in most setups. */
}
#else
#  include <QTextCodec>
inline void tb_set_utf8_locale()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}
#endif

#endif /* TB_QT_COMPAT_H */

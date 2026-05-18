#ifndef EB_QT_COMPAT_H
#define EB_QT_COMPAT_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  define EB_QT6 1
#else
#  define EB_QT6 0
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && !EB_QT6
#  define EB_QT5_MULTIMEDIA 1
#else
#  define EB_QT5_MULTIMEDIA 0
#endif

#if EB_QT6
#  define EB_QT6_MULTIMEDIA 1
#else
#  define EB_QT6_MULTIMEDIA 0
#endif

#if EB_QT6
#  include <QStringConverter>
inline void eb_set_utf8_locale()
{
    /* Qt 6 defaults to UTF-8 for QString and locale in most setups. */
}
#else
#  include <QTextCodec>
inline void eb_set_utf8_locale()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}
#endif

#endif /* EB_QT_COMPAT_H */

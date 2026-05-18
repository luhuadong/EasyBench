#ifndef EB_STARTUP_H
#define EB_STARTUP_H

#include <QString>
#include <QStringList>

class EbOptions;

struct EbStartupReport {
    QStringList warnings;
    QStringList errors;

    bool ok() const { return errors.isEmpty(); }
    QString summaryText() const;
};

EbStartupReport eb_check_runtime(const EbOptions &opt);

#endif /* EB_STARTUP_H */

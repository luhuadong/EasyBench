#ifndef TB_STARTUP_H
#define TB_STARTUP_H

#include <QString>
#include <QStringList>

class TbOptions;

struct TbStartupReport {
    QStringList warnings;
    QStringList errors;

    bool ok() const { return errors.isEmpty(); }
    QString summaryText() const;
};

TbStartupReport tb_check_runtime(const TbOptions &opt);

#endif /* TB_STARTUP_H */

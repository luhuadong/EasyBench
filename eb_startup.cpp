#include "eb_startup.h"
#include "eb_options.h"
#include "eb_common.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

QString EbStartupReport::summaryText() const
{
    QString text;
    if (!errors.isEmpty()) {
        text += QStringLiteral("错误：\n");
        for (const QString &line : errors) {
            text += QStringLiteral("  • ") + line + QLatin1Char('\n');
        }
    }
    if (!warnings.isEmpty()) {
        if (!text.isEmpty()) {
            text += QLatin1Char('\n');
        }
        text += QStringLiteral("警告：\n");
        for (const QString &line : warnings) {
            text += QStringLiteral("  • ") + line + QLatin1Char('\n');
        }
    }
    return text.trimmed();
}

static void checkPathReadable(const QString &path, const QString &label, EbStartupReport *report)
{
    if (path.isEmpty()) {
        report->errors << QStringLiteral("%1 路径未配置").arg(label);
        return;
    }
    if (!QFileInfo::exists(path)) {
        report->errors << QStringLiteral("%1 不存在：%2").arg(label, path);
    }
}

EbStartupReport eb_check_runtime(const EbOptions &opt)
{
    EbStartupReport report;

    if (!opt.configLoaded()) {
        report.errors << QStringLiteral(
            "未找到 easybench.conf（已搜索 /etc/gbox/、程序目录与当前工作目录）");
    }

    const QString backlight = opt.getBacklightNode();
    if (!backlight.isEmpty()) {
        const QString brightnessPath =
            QStringLiteral("/sys/class/backlight/%1/brightness").arg(backlight);
        if (!QFile::exists(brightnessPath)) {
            report.warnings << QStringLiteral("背光节点不可用：%1").arg(brightnessPath);
        }
    }

    if (!QFile::exists(QStringLiteral("/dev/fb0"))) {
        report.warnings << QStringLiteral("帧缓冲设备 /dev/fb0 不存在，将使用默认分辨率");
    }

    const QString serial = opt.getSerialPort();
    if (!serial.isEmpty() && !QFile::exists(serial)) {
        report.warnings << QStringLiteral("串口设备不存在：%1").arg(serial);
    }

    return report;
}

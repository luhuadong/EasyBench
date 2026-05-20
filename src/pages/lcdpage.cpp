#include "lcdpage.h"

#include <QFile>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QScreen>
#include <QVBoxLayout>
#include <QDebug>
#include <QtMath>

extern "C" {
#include <dirent.h>
}

namespace {

void setInfoLine(QLabel *label, const QString &name, const QString &value)
{
    label->setText(QStringLiteral("%1：%2").arg(name, value));
}

bool runCommand(const QString &program, const QStringList &args)
{
    return QProcess::execute(program, args) == 0;
}

QString detectXrandrOutputName()
{
    QProcess proc;
    proc.start(QStringLiteral("xrandr"), QStringList() << QStringLiteral("--current"));
    if (!proc.waitForFinished(2000) || proc.exitCode() != 0) {
        return QString();
    }
    const QStringList lines = QString::fromUtf8(proc.readAllStandardOutput()).split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (line.contains(QStringLiteral(" connected"))) {
            return line.section(QLatin1Char(' '), 0, 0).trimmed();
        }
    }
    return QString();
}

} // namespace

LcdPage::LcdPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
{
    setTitleLabelText(tr("显示与触摸测试"));
    buildUi();
    setupTouchButtons();
    refreshScreenInfo();
    detectBacklight();
    applyBacklightPercent(uiPercent);

}

void LcdPage::buildUi()
{
    QWidget *content = contentArea();

    lcdInfoBox = new QGroupBox(tr("屏幕信息"), content);
    infoDeviceLabel = new QLabel(lcdInfoBox);
    infoResolutionLabel = new QLabel(lcdInfoBox);
    infoVirtualLabel = new QLabel(lcdInfoBox);
    infoBppLabel = new QLabel(lcdInfoBox);
    infoPitchLabel = new QLabel(lcdInfoBox);
    infoPhysicalLabel = new QLabel(lcdInfoBox);
    infoGrayscaleLabel = new QLabel(lcdInfoBox);
    infoTouchLabel = new QLabel(lcdInfoBox);
    infoSourceLabel = new QLabel(lcdInfoBox);
    const QList<QLabel *> infoLabels = {infoDeviceLabel,    infoResolutionLabel, infoVirtualLabel,
                                          infoBppLabel,       infoPitchLabel,    infoPhysicalLabel,
                                          infoGrayscaleLabel, infoTouchLabel,    infoSourceLabel};
    for (QLabel *label : infoLabels) {
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    QVBoxLayout *infoLeftCol = new QVBoxLayout;
    infoLeftCol->setSpacing(6);
    infoLeftCol->addWidget(infoDeviceLabel);
    infoLeftCol->addWidget(infoResolutionLabel);
    infoLeftCol->addWidget(infoVirtualLabel);
    infoLeftCol->addWidget(infoBppLabel);
    infoLeftCol->addWidget(infoPitchLabel);
    infoLeftCol->addStretch();

    QVBoxLayout *infoRightCol = new QVBoxLayout;
    infoRightCol->setSpacing(6);
    infoRightCol->addWidget(infoPhysicalLabel);
    infoRightCol->addWidget(infoGrayscaleLabel);
    infoRightCol->addWidget(infoTouchLabel);
    infoRightCol->addWidget(infoSourceLabel);
    infoRightCol->addStretch();

    QHBoxLayout *infoColumns = new QHBoxLayout;
    infoColumns->setSpacing(32);
    infoColumns->addLayout(infoLeftCol, 1);
    infoColumns->addLayout(infoRightCol, 1);

    QVBoxLayout *infoBoxLayout = new QVBoxLayout(lcdInfoBox);
    infoBoxLayout->setContentsMargins(12, 16, 12, 12);
    infoBoxLayout->addLayout(infoColumns);

    QGroupBox *displayTestGroup = new QGroupBox(tr("显示测试"), content);
    pixelCheckBtn = new QPushButton(tr("像素测试"), displayTestGroup);
    pixelCheckBtn->setObjectName(QStringLiteral("functionBtn_small"));
    grayscaleTestBtn = new QPushButton(tr("灰度测试"), displayTestGroup);
    grayscaleTestBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QHBoxLayout *displayBtnRow = new QHBoxLayout;
    displayBtnRow->addWidget(pixelCheckBtn);
    displayBtnRow->addWidget(grayscaleTestBtn);
    displayBtnRow->addStretch();
    QVBoxLayout *displayLayout = new QVBoxLayout(displayTestGroup);
    displayLayout->setContentsMargins(12, 16, 12, 12);
    displayLayout->addLayout(displayBtnRow);

    QGroupBox *touchGroup = new QGroupBox(tr("触摸测试"), content);
    calBtn = new QPushButton(tr("4点校准"), touchGroup);
    calBtn->setObjectName(QStringLiteral("functionBtn_small"));
    linz9Btn = new QPushButton(tr("9点校准"), touchGroup);
    linz9Btn->setObjectName(QStringLiteral("functionBtn_small"));
    linzBtn = new QPushButton(tr("25点校准"), touchGroup);
    linzBtn->setObjectName(QStringLiteral("functionBtn_small"));
    drawBtn = new QPushButton(tr("轨迹测试"), touchGroup);
    drawBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QGridLayout *touchGrid = new QGridLayout;
    touchGrid->setSpacing(10);
    touchGrid->addWidget(calBtn, 0, 0);
    touchGrid->addWidget(linz9Btn, 0, 1);
    touchGrid->addWidget(linzBtn, 1, 0);
    touchGrid->addWidget(drawBtn, 1, 1);
    QVBoxLayout *touchLayout = new QVBoxLayout(touchGroup);
    touchLayout->setContentsMargins(12, 16, 12, 12);
    touchLayout->addLayout(touchGrid);

    QGroupBox *backlightGroup = new QGroupBox(tr("背光"), content);
    backlightLabel = new QLabel(backlightGroup);
    backlightSlider = new QSlider(Qt::Horizontal, backlightGroup);
    backlightSlider->setRange(1, 100);
    backlightSlider->setValue(uiPercent);
    backlightHintLabel = new QLabel(backlightGroup);
    backlightHintLabel->setWordWrap(true);
    backlightHintLabel->setObjectName(QStringLiteral("displayLabel"));
    QVBoxLayout *blLayout = new QVBoxLayout(backlightGroup);
    blLayout->setContentsMargins(12, 16, 12, 12);
    blLayout->addWidget(backlightLabel);
    blLayout->addWidget(backlightSlider);
    blLayout->addWidget(backlightHintLabel);

    QHBoxLayout *bottomRow = new QHBoxLayout;
    bottomRow->addWidget(displayTestGroup, 1);
    bottomRow->addWidget(touchGroup, 1);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(12);
    pageLayout->addWidget(lcdInfoBox);
    pageLayout->addLayout(bottomRow);
    pageLayout->addWidget(backlightGroup);

    connect(pixelCheckBtn, &QPushButton::clicked, this, &LcdPage::pixelCheckBtnClicked);
    connect(grayscaleTestBtn, &QPushButton::clicked, this, &LcdPage::grayscaleTestBtnClicked);
    connect(backlightSlider, &QSlider::valueChanged, this, &LcdPage::onBacklightSliderChanged);
    connect(calBtn, &QPushButton::clicked, this, &LcdPage::calBtnClicked);
    connect(linz9Btn, &QPushButton::clicked, this, &LcdPage::linz9BtnClicked);
    connect(linzBtn, &QPushButton::clicked, this, &LcdPage::linzBtnClicked);
    connect(drawBtn, &QPushButton::clicked, this, &LcdPage::drawBtnClicked);
}

void LcdPage::setupTouchButtons()
{
    const TouchType type = g_opt->getTouchType();
    if (type == TOUCH_NONE) {
        calBtn->setEnabled(false);
        linz9Btn->setEnabled(false);
        linzBtn->setEnabled(false);
        drawBtn->setEnabled(false);
    } else if (type == TOUCH_CAPACITIVE) {
        calBtn->setEnabled(false);
        linz9Btn->setEnabled(false);
        linzBtn->setEnabled(false);
        drawBtn->setEnabled(true);
    } else {
        calBtn->setEnabled(true);
        linz9Btn->setEnabled(true);
        linzBtn->setEnabled(true);
        drawBtn->setEnabled(true);
    }
    infoTouchLabel->setText(touchTypeText());
}

QString LcdPage::touchTypeText() const
{
    switch (g_opt->getTouchType()) {
    case TOUCH_RESISTIVE:
        return tr("电阻屏");
    case TOUCH_CAPACITIVE:
        return tr("电容屏");
    case TOUCH_OTHER:
        return tr("其他");
    case TOUCH_NONE:
    default:
        return tr("未配置 / 无触摸");
    }
}

bool LcdPage::loadFramebufferInfo()
{
    hasFramebuffer = false;
    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));

    const int fbfd = open("/dev/fb0", O_RDONLY);
    if (fbfd < 0) {
        return false;
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
        ::close(fbfd);
        return false;
    }
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
    ::close(fbfd);
    hasFramebuffer = true;
    return true;
}

void LcdPage::refreshScreenInfo()
{
    if (loadFramebufferInfo()) {
        applyScreenInfoLabels();
        return;
    }

    if (QGuiApplication::primaryScreen()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        qtScreenName = screen->name();
        qtScreenSize = screen->geometry().size();
        qtScreenDpi = qRound(screen->physicalDotsPerInch());
    }

    setInfoLine(infoDeviceLabel, tr("设备"),
                qtScreenName.isEmpty() ? tr("Qt 主屏幕") : qtScreenName);
    setInfoLine(infoResolutionLabel, tr("可视分辨率"),
                QStringLiteral("%1 × %2").arg(qtScreenSize.width()).arg(qtScreenSize.height()));
    setInfoLine(infoVirtualLabel, tr("虚拟分辨率"), tr("—"));
    setInfoLine(infoBppLabel, tr("色深"), tr("— (桌面合成)"));
    setInfoLine(infoPitchLabel, tr("行字节"), tr("—"));
    if (qtScreenDpi > 0) {
        const int wMm = qtScreenSize.width() * 254 / qtScreenDpi / 10;
        const int hMm = qtScreenSize.height() * 254 / qtScreenDpi / 10;
        setInfoLine(infoPhysicalLabel, tr("物理尺寸"),
                    tr("约 %1 × %2 mm（由 DPI 估算）").arg(wMm).arg(hMm));
    } else {
        setInfoLine(infoPhysicalLabel, tr("物理尺寸"), tr("—"));
    }
    setInfoLine(infoGrayscaleLabel, tr("灰度"), tr("—"));
    setInfoLine(infoTouchLabel, tr("触摸类型"), touchTypeText());
    setInfoLine(infoSourceLabel, tr("数据来源"), tr("Qt 屏幕 API（未检测到 /dev/fb0）"));
}

void LcdPage::applyScreenInfoLabels()
{
    setInfoLine(infoDeviceLabel, tr("设备"),
                QStringLiteral("/dev/fb0 (%1)").arg(QString::fromUtf8(finfo.id).trimmed()));
    setInfoLine(infoResolutionLabel, tr("可视分辨率"),
                QStringLiteral("%1 × %2").arg(vinfo.xres).arg(vinfo.yres));
    setInfoLine(infoVirtualLabel, tr("虚拟分辨率"),
                QStringLiteral("%1 × %2").arg(vinfo.xres_virtual).arg(vinfo.yres_virtual));
    setInfoLine(infoBppLabel, tr("色深"), QStringLiteral("%1 bpp").arg(vinfo.bits_per_pixel));
    setInfoLine(infoPitchLabel, tr("行字节"), QStringLiteral("%1 字节/行").arg(finfo.line_length));
    if (vinfo.width > 0 && vinfo.height > 0) {
        setInfoLine(infoPhysicalLabel, tr("物理尺寸"),
                    QStringLiteral("%1 × %2 mm").arg(vinfo.width).arg(vinfo.height));
    } else {
        setInfoLine(infoPhysicalLabel, tr("物理尺寸"), tr("未报告"));
    }
    setInfoLine(infoGrayscaleLabel, tr("灰度"), vinfo.grayscale ? tr("是") : tr("否"));
    setInfoLine(infoTouchLabel, tr("触摸类型"), touchTypeText());
    setInfoLine(infoSourceLabel, tr("数据来源"), tr("Linux 帧缓冲"));
}

bool LcdPage::detectBacklight()
{
    backlightNode = g_opt->getBacklightNode();
    if (backlightNode.isEmpty()) {
        DIR *dir = opendir("/sys/class/backlight");
        if (dir) {
            struct dirent *ent;
            while ((ent = readdir(dir)) != nullptr) {
                if (ent->d_name[0] == '.') {
                    continue;
                }
                backlightNode = QString::fromUtf8(ent->d_name);
                break;
            }
            closedir(dir);
        }
    }

    if (!backlightNode.isEmpty()) {
        brightnessPath = QStringLiteral("/sys/class/backlight/%1/brightness").arg(backlightNode);
        maxBrightnessPath = QStringLiteral("/sys/class/backlight/%1/max_brightness").arg(backlightNode);

        QFile maxFile(maxBrightnessPath);
        if (maxFile.open(QIODevice::ReadOnly)) {
            const int maxVal = QString::fromUtf8(maxFile.readAll()).trimmed().toInt();
            if (maxVal > 0) {
                hwMaxValue = maxVal;
            }
        }
        hwMinValue = 0;

        QFile brFile(brightnessPath);
        if (brFile.open(QIODevice::ReadOnly)) {
            hwCurrentValue = QString::fromUtf8(brFile.readAll()).trimmed().toInt();
            uiPercent = hardwareToPercent(hwCurrentValue);
            backlightMode = BacklightMode::HardwareSysfs;
            backlightHintLabel->setText(
                tr("硬件背光：%1（%2 ~ %3）").arg(backlightNode).arg(hwMinValue).arg(hwMaxValue));
            return true;
        }
    }

    if (runCommand(QStringLiteral("which"), QStringList() << QStringLiteral("xbacklight"))) {
        backlightMode = BacklightMode::XBacklight;
        backlightHintLabel->setText(tr("使用 xbacklight 调节桌面背光（无 sysfs 背光节点）"));
        return true;
    }

    const QString output = detectXrandrOutputName();
    if (!output.isEmpty()) {
        backlightMode = BacklightMode::Xrandr;
        backlightHintLabel->setText(tr("使用 xrandr 调节输出 %1 亮度").arg(output));
        return true;
    }

    backlightMode = BacklightMode::Simulated;
    backlightHintLabel->setText(
        tr("当前环境无硬件背光节点，滑块仅作演示；嵌入式设备请连接 /sys/class/backlight。"));
    return false;
}

int LcdPage::percentToHardware(int percent) const
{
    percent = qBound(1, percent, 100);
    if (hwMaxValue <= hwMinValue) {
        return percent;
    }
    return hwMinValue + (hwMaxValue - hwMinValue) * percent / 100;
}

int LcdPage::hardwareToPercent(int hwValue) const
{
    if (hwMaxValue <= hwMinValue) {
        return qBound(1, hwValue, 100);
    }
    return qBound(1, (hwValue - hwMinValue) * 100 / (hwMaxValue - hwMinValue), 100);
}

bool LcdPage::writeBacklightHardware(int hwValue)
{
    if (brightnessPath.isEmpty()) {
        return false;
    }
    QFile file(brightnessPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    file.write(QByteArray::number(hwValue));
    return true;
}

bool LcdPage::applyBacklightPercent(int percent)
{
    percent = qBound(1, percent, 100);
    uiPercent = percent;
    backlightSlider->blockSignals(true);
    backlightSlider->setValue(percent);
    backlightSlider->blockSignals(false);

    bool applied = false;
    switch (backlightMode) {
    case BacklightMode::HardwareSysfs:
        hwCurrentValue = percentToHardware(percent);
        applied = writeBacklightHardware(hwCurrentValue);
        break;
    case BacklightMode::XBacklight:
        applied = runCommand(QStringLiteral("xbacklight"), QStringList() << QStringLiteral("-set")
                                                                         << QString::number(percent));
        break;
    case BacklightMode::Xrandr: {
        const QString output = detectXrandrOutputName();
        if (!output.isEmpty()) {
            const double brightness = percent / 100.0;
            applied = runCommand(QStringLiteral("xrandr"),
                                 QStringList() << QStringLiteral("--output") << output
                                               << QStringLiteral("--brightness")
                                               << QString::number(brightness, 'f', 2));
        }
        break;
    }
    case BacklightMode::Simulated:
        applied = false;
        break;
    }

    backlightLabel->setText(tr("背光：%1%").arg(percent));
    if (!applied && backlightMode == BacklightMode::Simulated) {
        backlightLabel->setText(tr("背光（演示）：%1%").arg(percent));
    }
    return applied;
}

void LcdPage::onBacklightSliderChanged(int value)
{
    applyBacklightPercent(value);
}

void LcdPage::lcdBacklightUp()
{
    applyBacklightPercent(uiPercent + 5);
}

void LcdPage::lcdBacklightDown()
{
    applyBacklightPercent(uiPercent - 5);
}

void LcdPage::pixelCheckBtnClicked()
{
    colorBtn = new QPushButton;
    colorBtn->setObjectName(QStringLiteral("noneOutlineBtn"));
    colorBtn->setFixedSize(g_opt->lcdSize());
    colorBtn->setWindowFlags(Qt::FramelessWindowHint);
    colorBtn->setStyleSheet(QStringLiteral("background-color: #0000FF"));
    colorBtn->setCursor(Qt::BlankCursor);
    colorBtn->show();
    connect(colorBtn, &QPushButton::pressed, this, &LcdPage::changeColor);
}

void LcdPage::grayscaleTestBtnClicked()
{
    GrayscaleWidget *grayscaleWidget = new GrayscaleWidget(g_opt->lcdSize());
    grayscaleWidget->setAttribute(Qt::WA_DeleteOnClose);
    grayscaleWidget->show();
}

void LcdPage::changeColor()
{
    static int cnt = 1;
    if (!colorBtn) {
        return;
    }

    switch (cnt) {
    case 0:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #0000FF"));
        break;
    case 1:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #FF0000"));
        break;
    case 2:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #00FF00"));
        break;
    case 3:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #FFFF00"));
        break;
    case 4:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #00FFFF"));
        break;
    case 5:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #FF00FF"));
        break;
    case 6:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #808080"));
        break;
    case 7:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #FFFFFF"));
        break;
    case 8:
        colorBtn->setStyleSheet(QStringLiteral("background-color: #000000"));
        break;
    case 9:
        colorBtn->close();
        colorBtn = nullptr;
        cnt = 0;
        return;
    default:
        break;
    }
    cnt++;
}

void LcdPage::startECalib(const QString &mode)
{
    QProcess *proc = new QProcess(this);
    proc->setProgram(QStringLiteral("eCalib"));
    proc->setArguments(QStringList() << mode);
    if (!proc->startDetached()) {
        QMessageBox::warning(this, tr("触摸测试"),
                             tr("无法启动 eCalib（%1）。请确认已安装校准工具。").arg(mode));
        proc->deleteLater();
    }
}

void LcdPage::calBtnClicked()
{
    startECalib(QStringLiteral("Cal"));
}

void LcdPage::linz9BtnClicked()
{
    startECalib(QStringLiteral("Linz9"));
}

void LcdPage::linzBtnClicked()
{
    startECalib(QStringLiteral("Linz"));
}

void LcdPage::drawBtnClicked()
{
    startECalib(QStringLiteral("Draw"));
}

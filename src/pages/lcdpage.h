#ifndef LCDPAGE_H
#define LCDPAGE_H

#include "widgets/pagewidget.h"
#include "modules/lcd/grayscalewidget.h"
#include "tb_common.h"

#include <QGroupBox>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QString>

extern "C" {
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

class LcdPage : public PageWidget
{
    Q_OBJECT
public:
    explicit LcdPage(TbOptions *options, QWidget *parent = nullptr);

private slots:
    void pixelCheckBtnClicked();
    void grayscaleTestBtnClicked();
    void changeColor();
    void lcdBacklightUp();
    void lcdBacklightDown();
    void onBacklightSliderChanged(int value);
    void calBtnClicked();
    void linz9BtnClicked();
    void linzBtnClicked();
    void drawBtnClicked();
    void refreshScreenInfo();
    void startECalib(const QString &mode);

private:
    void buildUi();
    void setupTouchButtons();
    bool loadFramebufferInfo();
    void applyScreenInfoLabels();
    bool detectBacklight();
    bool writeBacklightHardware(int hwValue);
    bool applyBacklightPercent(int percent);
    int percentToHardware(int percent) const;
    int hardwareToPercent(int hwValue) const;
    QString touchTypeText() const;

    QPushButton *pixelCheckBtn = nullptr;
    QPushButton *grayscaleTestBtn = nullptr;
    QPushButton *colorBtn = nullptr;

    QPushButton *calBtn = nullptr;
    QPushButton *linz9Btn = nullptr;
    QPushButton *linzBtn = nullptr;
    QPushButton *drawBtn = nullptr;

    QLabel *backlightLabel = nullptr;
    QSlider *backlightSlider = nullptr;
    QLabel *backlightHintLabel = nullptr;

    QString backlightNode;
    QString brightnessPath;
    QString maxBrightnessPath;
    int hwMinValue = 1;
    int hwMaxValue = 100;
    int hwCurrentValue = 1;
    int uiPercent = 50;
    enum class BacklightMode { HardwareSysfs, XBacklight, Xrandr, Simulated };
    BacklightMode backlightMode = BacklightMode::Simulated;

    bool hasFramebuffer = false;
    struct fb_var_screeninfo vinfo {};
    struct fb_fix_screeninfo finfo {};

    QString qtScreenName;
    QSize qtScreenSize;
    int qtScreenDpi = 0;

    QGroupBox *lcdInfoBox = nullptr;
    QLabel *infoDeviceLabel = nullptr;
    QLabel *infoResolutionLabel = nullptr;
    QLabel *infoVirtualLabel = nullptr;
    QLabel *infoBppLabel = nullptr;
    QLabel *infoPitchLabel = nullptr;
    QLabel *infoPhysicalLabel = nullptr;
    QLabel *infoGrayscaleLabel = nullptr;
    QLabel *infoTouchLabel = nullptr;
    QLabel *infoSourceLabel = nullptr;
};

#endif /* LCDPAGE_H */

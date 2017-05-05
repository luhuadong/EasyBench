#ifndef LCDPAGE_H
#define LCDPAGE_H

#include "custom_widget/pagewidget.h"
#include "module/lcd/grayscalewidget.h"
#include "gyt_common.h"

#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
//#include <sys/mman.h>
#include <sys/ioctl.h>
}

#if defined(SYS_ADVANTECH)
#define MaxBacklightValue 32
#define MinBacklightValue 2
#elif defined(SYS_KONTRON)
#define MaxBacklightValue 100
#define MinBacklightValue 2
#endif

class LcdPage : public PageWidget
{
    Q_OBJECT
public:
    explicit LcdPage(QWidget *parent = 0);

public slots:

private slots:
    void pixelCheckBtnClicked();
    void grayscaleTestBtnClicked();
    void changeColor();
    void lcdBacklightUp();
    void lcdBacklightDown();

private:
    int getScreenInfo();

    QPushButton *pixelCheckBtn;
    QPushButton *grayscaleTestBtn;
    QPushButton *colorBtn;

    QLabel *backlightLabel;
    QProgressBar *backlightBar;
    QString backlightName;
    int backlightValue;
    int maxBacklightValue;
    int minBacklightValue;

    /*
     * visible resolution : xres yres
     * bits_per_pixel
     * grayscale
     * pixclock
     * height width
    */
    struct fb_var_screeninfo vinfo;
    QGroupBox *lcdInfoBox;
    QLabel *lcdInfoLabel;
    QLabel *resolutionLabel;
    QLabel *bppLabel;
    //QLabel *grayscaleLabel;
    //QLabel *pixclockLabel;
    //QLabel *sizeLabel;

};

#endif // LCDPAGE_H

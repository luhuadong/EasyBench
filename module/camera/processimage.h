#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include "videodevice.h"

typedef enum {
    CAMERA_NONE,
    CAMERA_ERROR,
    CAMERA_EXISTED,
    CAMERA_OPENED,
    CAMERA_CLOSED
}CameraStatus;

class ProcessImage : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessImage(QWidget *parent = 0);
    ~ProcessImage();

    CameraStatus cameraStatus;
    int openCamera(void);
    int closeCamera(void);
    int startCapture(void);
    int stopCapture(void);

private:
    QPainter *painter;
    QLabel *label;
    QImage *frame;
    //QPixmap *frame;
    QTimer *timer;
    int rs;
    uchar *pp;
    uchar * p;
    unsigned int len;
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    VideoDevice *vd;

    QHBoxLayout *hLayout;

signals:

public slots:

private slots:
    void paintEvent(QPaintEvent *);
    void display_error(QString err);

};

#endif // PROCESSIMAGE_H

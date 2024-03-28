#include "processimage.h"
#include "eb_common.h"
#include <QMessageBox>
#include <QTimer>
#include <QHBoxLayout>
#include <QDebug>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}


ProcessImage::ProcessImage(QWidget *parent) :
    QWidget(parent)
{
    pp = (unsigned char *)malloc(640 * 480/*QWidget::width()*QWidget::height()*/* 3 * sizeof(char));
    painter = new QPainter(this);
    frame = new QImage(pp, 640, 480, QImage::Format_RGB888);
    // frame = new QPixmap(640,320);
    label = new QLabel(this);
    //vd = new VideoDevice(QString(gVideoInputStr));
    timer = new QTimer(this);

    connect(vd, SIGNAL(display_error(QString)), this, SLOT(display_error(QString)));

    rs = openCamera();
    if(-1 == rs) {
        cameraStatus = CAMERA_NONE;
    }
    else {
        closeCamera();
        cameraStatus = CAMERA_EXISTED;
    }

    setFixedSize(640, 480);
    hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(label);
    setLayout(hLayout);
}

ProcessImage::~ProcessImage()
{
    closeCamera();
    /*
    rs = vd->stop_capturing();
    rs = vd->uninit_device();
    rs = vd->close_device();
    */
}

void ProcessImage::paintEvent(QPaintEvent *)
{
    rs = vd->get_frame((void **)&p, &len);
    convert_yuv_to_rgb_buffer(p, pp, 640, 480/*QWidget::width(),QWidget::height()*/);
    frame->loadFromData((uchar *)pp, /*len*/640 * 480 * 3 * sizeof(char));

    //    painter->begin(this);
    //    painter->drawImage(0,0,*frame);
    //    painter->end();
    //    rs = vd->unget_frame();
    // frame->load("./img3.jpg");

    label->setPixmap(QPixmap::fromImage(*frame, Qt::AutoColor));
    // label->show();
    rs = vd->unget_frame();
    // label->drawFrame();

    //    QPixmap *pixImage = new QPixmap();
    //    pixImage->loadFromData((uchar *)pp,sizeof(pp),0,Qt::AutoColor);
    //    QPainter painter(this);
    //    painter.begin(this);
    //    painter.drawPixmap(0,0,QWidget::width(),QWidget::height(),*pixImage);
    //    painter.end();
}

void ProcessImage::display_error(QString err)
{
    QMessageBox::warning(this,tr("error"), err,QMessageBox::Yes);
}

/*yuv格式转换为rgb格式*/
int ProcessImage::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 |
                                  yuv[in + 2] << 16 |
                                                 yuv[in + 1] <<  8 |
                                                                 yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

int ProcessImage::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}
/*yuv格式转换为rgb格式*/

int ProcessImage::openCamera()
{
    rs = vd->open_device();
    qDebug() << "Open Camera Device!";

    if(-1 == rs) {
        cameraStatus = CAMERA_NONE;
        return -1;
    }
    else {
        rs = vd->init_device();
        qDebug() << "Init Camera Device!";

        if(-1 == rs) {
            vd->close_device();
            cameraStatus = CAMERA_ERROR;
            return -1;
        }
        else {
            rs = vd->start_capturing();
            qDebug() << "Capturing from Camera!";

            if(-1==rs) {
                vd->uninit_device();
                vd->close_device();
                cameraStatus = CAMERA_ERROR;
                return -1;
            }

            connect(timer,SIGNAL(timeout()),this,SLOT(update()));
            timer->start(30);
            cameraStatus = CAMERA_OPENED;
            qDebug() << "Camera all is right!";
            return 0;
        }
    }
}

int ProcessImage::closeCamera()
{
    if(CAMERA_OPENED == cameraStatus) {
        timer->stop();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(update()));
        vd->stop_capturing();
        vd->uninit_device();
        vd->close_device();
        cameraStatus = CAMERA_CLOSED;
        qDebug() << "Close Camera Device!";
        return 0;
    }
    return -1;
}

int ProcessImage::startCapture()
{
    return vd->start_capturing();
}

int ProcessImage::stopCapture()
{
    return vd->stop_capturing();
}

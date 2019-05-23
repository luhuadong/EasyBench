#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "custom_widget/pagewidget.h"
//#include "module/camera/processimage.h"
#include "gyt_common.h"

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

#ifdef QT5_5_1
#include <QCameraViewfinderSettings>
#endif

class CameraPage : public PageWidget
{
    Q_OBJECT
public:
    explicit CameraPage(QWidget *parent = 0);

public slots:

private slots:
    void setCamera(const QCameraInfo &cameraInfo);
    void openCamera();
    void closeCamera();

    void displayCameraError();
    void updateCameraState(QCamera::State);

    void readyForCapture(bool ready);
    void processCapturedImage(int requestId, const QImage &img);
    void imageSaved(int id, const QString &fileName);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);


private:
    //ProcessImage *processImage;
    //QLabel *displayArea;

    struct resolution {
        u_int32_t w;
        u_int32_t h;
    } showResolution;

    QCamera             *camera;           // 读取摄像头
    QCameraViewfinder   *cameraViewfinder; // 渲染摄像头
    QCameraImageCapture *imageCapture;     // 获取摄像头当前帧

    bool isCapturingImage;

};

#endif // CAMERAPAGE_H

#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "custom_widget/pagewidget.h"
//#include "module/camera/processimage.h"
#include "gyt_common.h"

#include <QString>
#include <QWidget>
#include <QLabel>

#if QT_VERSION_5
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCameraViewfinderSettings>
#endif

class CameraPage : public PageWidget
{
    Q_OBJECT
public:
    explicit CameraPage(GytOptions *options, QWidget *parent = 0);

public slots:

private slots:
#if QT_VERSION_5
    void setCamera(const QCameraInfo &cameraInfo);
    void openCamera();
    void closeCamera();

    void displayCameraError();
    void updateCameraState(QCamera::State);

    void readyForCapture(bool ready);
    void processCapturedImage(int requestId, const QImage &img);
    void imageSaved(int id, const QString &fileName);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);
#endif

private:
    //ProcessImage *processImage;
    //QLabel *displayArea;

    struct resolution {
        u_int32_t w;
        u_int32_t h;
    } showResolution;

#if QT_VERSION_5
    QCamera             *camera;           // 读取摄像头
    QCameraViewfinder   *cameraViewfinder; // 渲染摄像头
    QCameraImageCapture *imageCapture;     // 获取摄像头当前帧
#endif
    bool isCapturingImage;

};

#endif // CAMERAPAGE_H

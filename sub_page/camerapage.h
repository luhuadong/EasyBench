#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "custom_widget/pagewidget.h"
#include "eb_common.h"
#include "eb_qt_compat.h"

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

#if EB_QT5_MULTIMEDIA
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCameraViewfinderSettings>
#endif

#if EB_QT6_MULTIMEDIA
#include <QCamera>
#include <QCameraDevice>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoWidget>
#endif

class CameraPage : public PageWidget
{
    Q_OBJECT
public:
    explicit CameraPage(EbOptions *options, QWidget *parent = 0);

public slots:

private slots:
#if EB_QT5_MULTIMEDIA
    void setCamera(const QCameraInfo &cameraInfo);
    void videoDeviceBoxCurrentIndexChanged(int index);
    void openCamera();
    void closeCamera();
    void toggleLock();

    void displayCameraError();
    void updateCameraState(QCamera::State);

    void readyForCapture(bool ready);
    void processCapturedImage(int requestId, const QImage &img);
    void imageSaved(int id, const QString &fileName);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);
#endif

#if EB_QT6_MULTIMEDIA
    void setCameraDevice(const QCameraDevice &device);
    void videoDeviceBoxCurrentIndexChanged(int index);
    void openCamera();
    void closeCamera();

    void displayCameraError(QCamera::Error error, const QString &errorString);
    void updateCameraState(bool active);
    void displayCaptureError(int id, QImageCapture::Error error, const QString &errorString);
#endif

private:
    void initVideoDeviceBox();

    QSize showResolution;

#if EB_QT5_MULTIMEDIA || EB_QT6_MULTIMEDIA
    QComboBox *videoDeviceBox;
    QPushButton *lockBtn;
#endif

#if EB_QT5_MULTIMEDIA
    QCamera             *camera;
    QCameraViewfinder   *cameraViewfinder;
    QCameraImageCapture *imageCapture;
#endif

#if EB_QT6_MULTIMEDIA
    QCamera               *camera;
    QMediaCaptureSession  *captureSession;
    QVideoWidget          *videoWidget;
    QImageCapture         *imageCapture;
#endif

    bool isCapturingImage;

};

#endif // CAMERAPAGE_H

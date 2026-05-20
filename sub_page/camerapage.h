#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "custom_widget/pagewidget.h"
#include "eb_qt_compat.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include <QGroupBox>

class QStackedWidget;

#if EB_QT5_MULTIMEDIA
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
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

    bool hasMultimediaSupport() const;
    bool isCameraActive() const { return cameraActive; }
    bool canOpenCamera() const;
    bool canCaptureStill() const;

public slots:
    void toggleCamera();
    void captureStillImage();

private slots:
    void refreshDeviceList();
    void onDeviceChanged(int index);
    void onResolutionChanged(int index);
    void openCamera();
    void closeCamera();

#if EB_QT5_MULTIMEDIA
    void onCameraStateChanged(QCamera::State state);
    void onCameraError();
    void onCaptureReadyChanged(bool ready);
    void onImageCaptured(int id, const QImage &image);
    void onImageSaved(int id, const QString &path);
    void onCaptureError(int id, QCameraImageCapture::Error error, const QString &message);
#endif

#if EB_QT6_MULTIMEDIA
    void onCameraActiveChanged(bool active);
    void onCameraError(QCamera::Error error, const QString &message);
    void onCaptureError(int id, QImageCapture::Error error, const QString &message);
#endif

private:
    void buildUi();
    void setStatusText(const QString &text);
    void updateControlStates();
    void populateResolutionList();
    void applySelectedResolution();
    void teardownCamera();
    void setupCameraForCurrentDevice();

#if EB_QT5_MULTIMEDIA
    QCameraInfo currentCameraInfo() const;
    void attachCamera(const QCameraInfo &info);
#endif

#if EB_QT6_MULTIMEDIA
    QCameraDevice currentCameraDevice() const;
    void attachCamera(const QCameraDevice &device);
#endif

    QGroupBox *controlGroup;
    QComboBox *deviceBox;
    QComboBox *resolutionBox;
    QLabel *statusLabel;
    QLabel *deviceInfoLabel;
    QPushButton *refreshDevicesBtn;
    QPushButton *cameraToggleBtn = nullptr;
    QPushButton *cameraCaptureBtn = nullptr;

    QFrame *previewFrame;
    QStackedWidget *previewStack;
    QLabel *previewPlaceholder;
    QLabel *previewInfoLabel;

#if EB_QT5_MULTIMEDIA
    QCameraViewfinder *viewfinder;
    QCamera *camera;
    QCameraImageCapture *imageCapture;
#endif

#if EB_QT6_MULTIMEDIA
    QVideoWidget *videoWidget;
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QImageCapture *imageCapture;
#endif

    bool cameraActive;
    bool multimediaAvailable;
};

#endif // CAMERAPAGE_H

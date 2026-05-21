#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "widgets/pagewidget.h"
#include "eb_qt_compat.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include <QGroupBox>

class QResizeEvent;

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
    ~CameraPage() override;

    QString defaultStatusHint() const override;

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
    void resizeEvent(QResizeEvent *event) override;
    void updatePreviewBannerSize();
    void setStatusText(const QString &text);
    void updateStatusBar();
    void updateControlStates();
    void populateResolutionList();
    void applySelectedResolution();
    void teardownCamera();
    void setupCameraForCurrentDevice();
    void ensurePreviewVideoWidget();
    void releasePreviewVideoWidget();
    void updateSelectedDeviceDetail();

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
    QPushButton *refreshDevicesBtn;
    QPushButton *cameraToggleBtn = nullptr;
    QPushButton *cameraCaptureBtn = nullptr;

    QWidget *previewHost;
    QFrame *previewFrame;
    QStackedWidget *previewStack;
    QLabel *previewPlaceholder;

#if EB_QT5_MULTIMEDIA
    QCameraViewfinder *viewfinder = nullptr;
    QCamera *camera = nullptr;
    QCameraImageCapture *imageCapture = nullptr;
#endif

#if EB_QT6_MULTIMEDIA
    QVideoWidget *videoWidget = nullptr;
    QCamera *camera = nullptr;
    QMediaCaptureSession *captureSession;
    QImageCapture *imageCapture = nullptr;
#endif

    bool cameraActive;
    bool multimediaAvailable;
    QString selectedDeviceDetail;
};

#endif // CAMERAPAGE_H

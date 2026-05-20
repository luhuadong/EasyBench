#include "camerapage.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QResizeEvent>
#include <QStackedWidget>

#if EB_QT5_MULTIMEDIA
Q_DECLARE_METATYPE(QCameraInfo)
#endif

#if EB_QT6_MULTIMEDIA
Q_DECLARE_METATYPE(QCameraDevice)
#endif

CameraPage::CameraPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
    , controlGroup(nullptr)
    , deviceBox(nullptr)
    , resolutionBox(nullptr)
    , refreshDevicesBtn(nullptr)
    , previewHost(nullptr)
    , previewFrame(nullptr)
    , previewStack(nullptr)
    , previewPlaceholder(nullptr)
#if EB_QT5_MULTIMEDIA
    , viewfinder(nullptr)
    , camera(nullptr)
    , imageCapture(nullptr)
#endif
#if EB_QT6_MULTIMEDIA
    , videoWidget(nullptr)
    , camera(nullptr)
    , captureSession(nullptr)
    , imageCapture(nullptr)
#endif
    , cameraActive(false)
#if EB_QT5_MULTIMEDIA || EB_QT6_MULTIMEDIA
    , multimediaAvailable(true)
#else
    , multimediaAvailable(false)
#endif
{
    setTitleLabelText(tr("摄像头测试"));

    buildUi();

    connect(deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceChanged(int)));
    connect(resolutionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onResolutionChanged(int)));
    connect(refreshDevicesBtn, SIGNAL(clicked()), this, SLOT(refreshDeviceList()));

#if EB_QT6_MULTIMEDIA
    captureSession = new QMediaCaptureSession(this);
#endif

    refreshDeviceList();
    updateControlStates();
    setStatusMessage(defaultStatusHint());
}

QString CameraPage::defaultStatusHint() const
{
    return tr("就绪");
}

void CameraPage::buildUi()
{
    QWidget *content = contentArea();

    controlGroup = new QGroupBox(tr("设置"), content);
    deviceBox = new QComboBox(controlGroup);
    deviceBox->setMinimumWidth(200);
    resolutionBox = new QComboBox(controlGroup);
    resolutionBox->setMinimumWidth(140);
    refreshDevicesBtn = new QPushButton(tr("刷新设备列表"), controlGroup);
    refreshDevicesBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraToggleBtn = new QPushButton(tr("打开"), controlGroup);
    cameraToggleBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraCaptureBtn = new QPushButton(tr("拍照"), controlGroup);
    cameraCaptureBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraCaptureBtn->setEnabled(false);
    connect(cameraToggleBtn, &QPushButton::clicked, this, &CameraPage::toggleCamera);
    connect(cameraCaptureBtn, &QPushButton::clicked, this, &CameraPage::captureStillImage);

    QGridLayout *controlGrid = new QGridLayout(controlGroup);
    controlGrid->setContentsMargins(12, 14, 12, 12);
    controlGrid->setHorizontalSpacing(10);
    controlGrid->setVerticalSpacing(8);
    controlGrid->addWidget(new QLabel(tr("摄像头"), controlGroup), 0, 0);
    controlGrid->addWidget(deviceBox, 0, 1);
    controlGrid->addWidget(new QLabel(tr("分辨率"), controlGroup), 0, 2);
    controlGrid->addWidget(resolutionBox, 0, 3);
    controlGrid->addWidget(refreshDevicesBtn, 0, 4);
    controlGrid->addWidget(cameraToggleBtn, 0, 5);
    controlGrid->addWidget(cameraCaptureBtn, 0, 6);
    controlGrid->setColumnStretch(1, 1);
    controlGrid->setColumnStretch(3, 0);

    previewFrame = new QFrame(content);
    previewFrame->setObjectName(QStringLiteral("cameraPreviewFrame"));
    previewFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    previewPlaceholder = new QLabel(tr("预览区域\n\n选择设备后点击「打开」开始预览"), previewFrame);
    previewPlaceholder->setAlignment(Qt::AlignCenter);
    previewPlaceholder->setObjectName(QStringLiteral("cameraPreviewPlaceholder"));

#if EB_QT5_MULTIMEDIA
    viewfinder = new QCameraViewfinder(previewFrame);
    viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewfinder->hide();
#endif

#if EB_QT6_MULTIMEDIA
    videoWidget = new QVideoWidget(previewFrame);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoWidget->hide();
#endif

    previewStack = new QStackedWidget(previewFrame);
    previewStack->addWidget(previewPlaceholder);
#if EB_QT5_MULTIMEDIA
    previewStack->addWidget(viewfinder);
#elif EB_QT6_MULTIMEDIA
    previewStack->addWidget(videoWidget);
#else
    previewStack->addWidget(new QWidget(previewFrame));
#endif

    QVBoxLayout *previewLayout = new QVBoxLayout(previewFrame);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->addWidget(previewStack);

    previewHost = new QWidget(content);
    previewHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *previewHostLayout = new QVBoxLayout(previewHost);
    previewHostLayout->setContentsMargins(0, 0, 0, 0);
    previewHostLayout->setSpacing(0);

    QWidget *previewBannerRow = new QWidget(previewHost);
    QHBoxLayout *bannerLayout = new QHBoxLayout(previewBannerRow);
    bannerLayout->setContentsMargins(0, 0, 0, 0);
    bannerLayout->addStretch();
    bannerLayout->addWidget(previewFrame, 0, Qt::AlignCenter);
    bannerLayout->addStretch();

    previewHostLayout->addStretch(1);
    previewHostLayout->addWidget(previewBannerRow, 0, Qt::AlignHCenter);
    previewHostLayout->addStretch(1);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(16, 8, 16, 8);
    pageLayout->setSpacing(8);
    pageLayout->addWidget(previewHost, 1);
    pageLayout->addWidget(controlGroup, 0);

    updatePreviewBannerSize();
}

void CameraPage::resizeEvent(QResizeEvent *event)
{
    PageWidget::resizeEvent(event);
    updatePreviewBannerSize();
}

void CameraPage::updatePreviewBannerSize()
{
    if (!previewHost || !previewFrame) {
        return;
    }

    const int margin = 8;
    const int maxWidth = qMin(720, CONTENT_WIDTH - 48);
    int availW = previewHost->width() - margin;
    int availH = previewHost->height() - margin;
    if (availW <= 0 || availH <= 0) {
        return;
    }

    int w = qMin(availW, maxWidth);
    int h = w * 3 / 4;
    if (h > availH) {
        h = availH;
        w = h * 4 / 3;
    }
    w = qMax(w, 320);
    h = qMax(h, 240);

    previewFrame->setFixedSize(w, h);
}

void CameraPage::setStatusText(const QString &text)
{
    setStatusMessage(text);
}

void CameraPage::updateStatusBar()
{
    if (!multimediaAvailable) {
        setStatusMessage(tr("当前构建未启用多媒体模块"));
        return;
    }

    if (deviceBox->count() == 0) {
        setStatusMessage(tr("未检测到摄像头设备"));
        return;
    }

    QString msg;
    if (cameraActive) {
        const QSize res = resolutionBox->currentData().toSize();
        if (res.isValid()) {
            msg = tr("预览中 · %1 × %2").arg(res.width()).arg(res.height());
        } else {
            msg = tr("预览中");
        }
    } else if (resolutionBox->currentIndex() >= 0) {
        const QSize res = resolutionBox->currentData().toSize();
        if (res.isValid()) {
            msg = tr("分辨率 %1 × %2，点击「打开」预览").arg(res.width()).arg(res.height());
        } else {
            msg = tr("点击「打开」开始预览");
        }
    } else {
        msg = defaultStatusHint();
    }

    if (!selectedDeviceDetail.isEmpty()) {
        msg = selectedDeviceDetail + QStringLiteral(" — ") + msg;
    }
    setStatusMessage(msg);
}

bool CameraPage::hasMultimediaSupport() const
{
    return multimediaAvailable;
}

bool CameraPage::canOpenCamera() const
{
    return multimediaAvailable && deviceBox->count() > 0 && !cameraActive;
}

bool CameraPage::canCaptureStill() const
{
    if (!cameraActive) {
        return false;
    }
#if EB_QT5_MULTIMEDIA
    return imageCapture && imageCapture->isReadyForCapture();
#elif EB_QT6_MULTIMEDIA
    return imageCapture != nullptr;
#else
    return false;
#endif
}

void CameraPage::toggleCamera()
{
    if (cameraActive) {
        closeCamera();
    } else {
        openCamera();
    }
}

void CameraPage::updateControlStates()
{
    const bool hasDevice = deviceBox->count() > 0 && multimediaAvailable;
    deviceBox->setEnabled(!cameraActive && hasDevice);
    resolutionBox->setEnabled(hasDevice);
    refreshDevicesBtn->setEnabled(!cameraActive);

    if (previewStack) {
        previewStack->setCurrentIndex(cameraActive ? 1 : 0);
    }

    if (cameraToggleBtn && cameraCaptureBtn) {
        if (!multimediaAvailable) {
            cameraToggleBtn->setEnabled(false);
            cameraCaptureBtn->setEnabled(false);
        } else if (cameraActive) {
            cameraToggleBtn->setText(tr("关闭"));
            cameraToggleBtn->setEnabled(true);
            cameraCaptureBtn->setEnabled(canCaptureStill());
        } else {
            cameraToggleBtn->setText(tr("打开"));
            cameraToggleBtn->setEnabled(canOpenCamera());
            cameraCaptureBtn->setEnabled(false);
        }
    }
}

void CameraPage::refreshDeviceList()
{
    if (!multimediaAvailable) {
        setStatusText(tr("当前构建未启用多媒体模块"));
        return;
    }

    if (cameraActive) {
        closeCamera();
    }

    teardownCamera();

    const int previousIndex = deviceBox->currentIndex();
    deviceBox->blockSignals(true);
    deviceBox->clear();

#if EB_QT5_MULTIMEDIA
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &info : cameras) {
        const QString label = info.description().isEmpty()
            ? info.deviceName()
            : QStringLiteral("%1 (%2)").arg(info.description(), info.deviceName());
        deviceBox->addItem(label, QVariant::fromValue(info));
    }
#elif EB_QT6_MULTIMEDIA
    const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    for (const QCameraDevice &device : devices) {
        deviceBox->addItem(device.description(), QVariant::fromValue(device));
    }
#endif

    deviceBox->blockSignals(false);

    if (deviceBox->count() == 0) {
        selectedDeviceDetail.clear();
        resolutionBox->clear();
        updateControlStates();
        updateStatusBar();
        return;
    }

    if (previousIndex >= 0 && previousIndex < deviceBox->count()) {
        deviceBox->setCurrentIndex(previousIndex);
    } else {
        deviceBox->setCurrentIndex(0);
    }

    onDeviceChanged(deviceBox->currentIndex());
}

void CameraPage::onDeviceChanged(int index)
{
    if (index < 0) {
        return;
    }

    if (cameraActive) {
        closeCamera();
    }

    setupCameraForCurrentDevice();
    updateControlStates();
    updateStatusBar();
}

void CameraPage::onResolutionChanged(int index)
{
    Q_UNUSED(index);
    applySelectedResolution();
    updateStatusBar();
}

void CameraPage::populateResolutionList()
{
    resolutionBox->blockSignals(true);
    resolutionBox->clear();

    const QSize preferred = g_opt->cameraViewSize();

#if EB_QT5_MULTIMEDIA
    if (!camera) {
        resolutionBox->addItem(QStringLiteral("%1 x %2").arg(preferred.width()).arg(preferred.height()),
                               preferred);
    } else {
        const QList<QCameraViewfinderSettings> supported = camera->supportedViewfinderSettings();
        if (supported.isEmpty()) {
            resolutionBox->addItem(QStringLiteral("%1 x %2").arg(preferred.width()).arg(preferred.height()),
                                   preferred);
        } else {
            int preferredIndex = 0;
            for (int i = 0; i < supported.size(); ++i) {
                const QSize res = supported.at(i).resolution();
                resolutionBox->addItem(QStringLiteral("%1 x %2").arg(res.width()).arg(res.height()), res);
                if (res == preferred) {
                    preferredIndex = i;
                }
            }
            resolutionBox->setCurrentIndex(preferredIndex);
        }
    }
#else
    resolutionBox->addItem(QStringLiteral("%1 x %2").arg(preferred.width()).arg(preferred.height()), preferred);
#endif

    resolutionBox->blockSignals(false);
}

void CameraPage::applySelectedResolution()
{
#if EB_QT5_MULTIMEDIA
    if (!camera || resolutionBox->currentIndex() < 0) {
        return;
    }
    const QSize res = resolutionBox->currentData().toSize();
    if (!res.isValid()) {
        return;
    }
    QCameraViewfinderSettings settings;
    settings.setResolution(res);
    camera->setViewfinderSettings(settings);
#endif
}

void CameraPage::teardownCamera()
{
#if EB_QT5_MULTIMEDIA
    delete imageCapture;
    imageCapture = nullptr;
    delete camera;
    camera = nullptr;
#elif EB_QT6_MULTIMEDIA
    if (captureSession) {
        captureSession->setCamera(nullptr);
        captureSession->setImageCapture(nullptr);
    }
    delete imageCapture;
    imageCapture = nullptr;
    delete camera;
    camera = nullptr;
#endif
}

void CameraPage::setupCameraForCurrentDevice()
{
#if EB_QT5_MULTIMEDIA
    attachCamera(currentCameraInfo());
#elif EB_QT6_MULTIMEDIA
    attachCamera(currentCameraDevice());
#endif
}

#if EB_QT5_MULTIMEDIA

QCameraInfo CameraPage::currentCameraInfo() const
{
    if (deviceBox->currentIndex() < 0) {
        return QCameraInfo();
    }
    return deviceBox->currentData().value<QCameraInfo>();
}

void CameraPage::attachCamera(const QCameraInfo &info)
{
    teardownCamera();

    if (info.isNull()) {
        selectedDeviceDetail.clear();
        updateStatusBar();
        return;
    }

    camera = new QCamera(info, this);
    imageCapture = new QCameraImageCapture(camera, this);

    camera->setViewfinder(viewfinder);
    camera->setCaptureMode(QCamera::CaptureStillImage);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(onCameraStateChanged(QCamera::State)));
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(onCameraError()));
    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(onCaptureReadyChanged(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(onImageCaptured(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(onImageSaved(int,QString)));
    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)),
            this, SLOT(onCaptureError(int,QCameraImageCapture::Error,QString)));

    selectedDeviceDetail = info.description().isEmpty()
        ? info.deviceName()
        : QStringLiteral("%1 (%2)").arg(info.description(), info.deviceName());

    populateResolutionList();
    applySelectedResolution();
    updateStatusBar();
}

void CameraPage::onCameraStateChanged(QCamera::State state)
{
    switch (state) {
    case QCamera::ActiveState:
    case QCamera::LoadedState:
    case QCamera::UnloadedState:
        updateStatusBar();
        break;
    default:
        break;
    }
}

void CameraPage::onCameraError()
{
    if (!camera) {
        return;
    }
    setStatusText(tr("错误：%1").arg(camera->errorString()));
    QMessageBox::warning(this, tr("摄像头错误"), camera->errorString());
}

void CameraPage::onCaptureReadyChanged(bool ready)
{
    Q_UNUSED(ready);
    updateControlStates();
}

void CameraPage::onImageCaptured(int id, const QImage &image)
{
    Q_UNUSED(id);
    Q_UNUSED(image);
}

void CameraPage::onImageSaved(int id, const QString &path)
{
    Q_UNUSED(id);
    setStatusText(tr("照片已保存：%1").arg(path));
    QMessageBox::information(this, tr("拍照成功"), tr("已保存至：\n%1").arg(path));
}

void CameraPage::onCaptureError(int id, QCameraImageCapture::Error error, const QString &message)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    setStatusText(tr("拍照失败：%1").arg(message));
    QMessageBox::warning(this, tr("拍照失败"), message);
}

#elif EB_QT6_MULTIMEDIA

QCameraDevice CameraPage::currentCameraDevice() const
{
    if (deviceBox->currentIndex() < 0) {
        return QCameraDevice();
    }
    return deviceBox->currentData().value<QCameraDevice>();
}

void CameraPage::attachCamera(const QCameraDevice &device)
{
    teardownCamera();

    if (device.isNull()) {
        selectedDeviceDetail.clear();
        updateStatusBar();
        return;
    }

    camera = new QCamera(device, this);
    imageCapture = new QImageCapture(this);
    captureSession->setCamera(camera);
    captureSession->setVideoOutput(videoWidget);
    captureSession->setImageCapture(imageCapture);

    connect(camera, &QCamera::activeChanged, this, &CameraPage::onCameraActiveChanged);
    connect(camera, &QCamera::errorOccurred, this, &CameraPage::onCameraError);
    connect(imageCapture, &QImageCapture::errorOccurred, this, &CameraPage::onCaptureError);

    selectedDeviceDetail = device.description();
    if (selectedDeviceDetail.isEmpty()) {
        selectedDeviceDetail = device.id();
    }
    populateResolutionList();
    updateStatusBar();
}

void CameraPage::onCameraActiveChanged(bool active)
{
    Q_UNUSED(active);
    updateControlStates();
    updateStatusBar();
}

void CameraPage::onCameraError(QCamera::Error error, const QString &message)
{
    Q_UNUSED(error);
    setStatusText(tr("错误：%1").arg(message));
    QMessageBox::warning(this, tr("摄像头错误"), message);
}

void CameraPage::onCaptureError(int id, QImageCapture::Error error, const QString &message)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    setStatusText(tr("拍照失败：%1").arg(message));
    QMessageBox::warning(this, tr("拍照失败"), message);
}

#endif

void CameraPage::openCamera()
{
    if (!multimediaAvailable || deviceBox->count() == 0) {
        QMessageBox::information(this, tr("摄像头"), tr("没有可用的摄像头设备。"));
        return;
    }

#if EB_QT5_MULTIMEDIA || EB_QT6_MULTIMEDIA
    if (!camera) {
        setupCameraForCurrentDevice();
    }
    if (!camera) {
        return;
    }

    applySelectedResolution();
    camera->start();
    cameraActive = true;
    updateControlStates();
    setStatusMessage(tr("正在启动预览…"));
#endif
}

void CameraPage::closeCamera()
{
#if EB_QT5_MULTIMEDIA || EB_QT6_MULTIMEDIA
    if (!camera || !cameraActive) {
        return;
    }
    camera->stop();
    cameraActive = false;
    updateControlStates();
    updateStatusBar();
#endif
}

void CameraPage::captureStillImage()
{
#if EB_QT5_MULTIMEDIA
    if (!cameraActive || !imageCapture || !imageCapture->isReadyForCapture()) {
        QMessageBox::information(this, tr("拍照"), tr("请先打开摄像头并等待就绪。"));
        return;
    }

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir().mkpath(dir);
    const QString path = dir + QStringLiteral("/easybench_%1.jpg")
        .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_hhmmss")));
    imageCapture->capture(path);
    setStatusText(tr("正在拍照…"));
#elif EB_QT6_MULTIMEDIA
    if (!cameraActive || !imageCapture) {
        QMessageBox::information(this, tr("拍照"), tr("请先打开摄像头。"));
        return;
    }
    imageCapture->captureToFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
        + QStringLiteral("/easybench_")
        + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_hhmmss"))
        + QStringLiteral(".jpg"));
    setStatusText(tr("正在拍照…"));
#else
    Q_UNUSED(this);
#endif
}

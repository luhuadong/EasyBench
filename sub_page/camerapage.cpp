#include "camerapage.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
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
    , statusLabel(nullptr)
    , deviceInfoLabel(nullptr)
    , refreshDevicesBtn(nullptr)
    , previewFrame(nullptr)
    , previewStack(nullptr)
    , previewPlaceholder(nullptr)
    , previewInfoLabel(nullptr)
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
}

void CameraPage::buildUi()
{
    QWidget *content = contentArea();

    controlGroup = new QGroupBox(tr("设备与参数"), content);
    deviceBox = new QComboBox(controlGroup);
    deviceBox->setMinimumWidth(220);
    resolutionBox = new QComboBox(controlGroup);
    resolutionBox->setMinimumWidth(220);
    statusLabel = new QLabel(tr("状态：未打开"), controlGroup);
    statusLabel->setWordWrap(true);
    deviceInfoLabel = new QLabel(controlGroup);
    deviceInfoLabel->setWordWrap(true);
    deviceInfoLabel->setObjectName(QStringLiteral("displayLabel"));
    refreshDevicesBtn = new QPushButton(tr("刷新设备列表"), controlGroup);
    refreshDevicesBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraToggleBtn = new QPushButton(tr("打开"), controlGroup);
    cameraToggleBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraCaptureBtn = new QPushButton(tr("拍照"), controlGroup);
    cameraCaptureBtn->setObjectName(QStringLiteral("functionBtn_small"));
    cameraCaptureBtn->setEnabled(false);
    connect(cameraToggleBtn, &QPushButton::clicked, this, &CameraPage::toggleCamera);
    connect(cameraCaptureBtn, &QPushButton::clicked, this, &CameraPage::captureStillImage);

    QHBoxLayout *actionRow = new QHBoxLayout;
    actionRow->addWidget(cameraToggleBtn);
    actionRow->addWidget(cameraCaptureBtn);
    actionRow->addStretch();

    QFormLayout *form = new QFormLayout;
    form->setContentsMargins(12, 16, 12, 12);
    form->setSpacing(10);
    form->addRow(tr("摄像头"), deviceBox);
    form->addRow(tr("分辨率"), resolutionBox);
    form->addRow(tr("状态"), statusLabel);
    form->addRow(tr("详情"), deviceInfoLabel);
    form->addRow(QString(), refreshDevicesBtn);
    form->addRow(QString(), actionRow);
    controlGroup->setLayout(form);
    controlGroup->setFixedWidth(280);

    previewFrame = new QFrame(content);
    previewFrame->setObjectName(QStringLiteral("cameraPreviewFrame"));
    previewFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    previewFrame->setMinimumSize(400, 300);

    previewPlaceholder = new QLabel(tr("预览区域\n\n选择设备后点击「打开」开始预览"), previewFrame);
    previewPlaceholder->setAlignment(Qt::AlignCenter);
    previewPlaceholder->setObjectName(QStringLiteral("cameraPreviewPlaceholder"));

    previewInfoLabel = new QLabel(previewFrame);
    previewInfoLabel->setAlignment(Qt::AlignCenter);
    previewInfoLabel->setObjectName(QStringLiteral("displayLabel"));

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
    previewLayout->setContentsMargins(8, 8, 8, 8);
    previewLayout->setSpacing(8);
    previewLayout->addWidget(previewStack, 1);
    previewLayout->addWidget(previewInfoLabel);

    QHBoxLayout *pageLayout = new QHBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(16);
    pageLayout->addWidget(controlGroup, 0);
    pageLayout->addWidget(previewFrame, 1);
}

void CameraPage::setStatusText(const QString &text)
{
    statusLabel->setText(tr("状态：%1").arg(text));
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
        deviceInfoLabel->setText(tr("未检测到摄像头设备"));
        resolutionBox->clear();
        previewInfoLabel->clear();
        setStatusText(tr("无可用设备"));
        updateControlStates();
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
}

void CameraPage::onResolutionChanged(int index)
{
    Q_UNUSED(index);
    if (!cameraActive) {
        return;
    }
    applySelectedResolution();
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
    previewInfoLabel->setText(tr("预览分辨率：%1 x %2").arg(res.width()).arg(res.height()));
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
        deviceInfoLabel->setText(tr("无效设备"));
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

    deviceInfoLabel->setText(QStringLiteral("%1\n%2")
        .arg(info.description(), info.deviceName()));

    populateResolutionList();
    applySelectedResolution();
    setStatusText(tr("已选择设备，点击「打开」预览"));
}

void CameraPage::onCameraStateChanged(QCamera::State state)
{
    switch (state) {
    case QCamera::ActiveState:
        setStatusText(tr("预览中"));
        break;
    case QCamera::LoadedState:
        setStatusText(tr("已加载"));
        break;
    case QCamera::UnloadedState:
        setStatusText(tr("已卸载"));
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
        deviceInfoLabel->setText(tr("无效设备"));
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

    deviceInfoLabel->setText(device.description());
    populateResolutionList();

    const QSize preferred = g_opt->cameraViewSize();
    previewInfoLabel->setText(tr("预览分辨率：%1 x %2").arg(preferred.width()).arg(preferred.height()));
    setStatusText(tr("已选择设备，点击「打开」预览"));
}

void CameraPage::onCameraActiveChanged(bool active)
{
    setStatusText(active ? tr("预览中") : tr("已停止"));
    updateControlStates();
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
    setStatusText(tr("正在启动预览…"));
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
    setStatusText(tr("已关闭"));
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

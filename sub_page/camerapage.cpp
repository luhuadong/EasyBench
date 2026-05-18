#include "camerapage.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QAction>
#include <QVariant>
#include <QDebug>

#if EB_QT5_MULTIMEDIA
Q_DECLARE_METATYPE(QCameraInfo)
#endif

#if EB_QT6_MULTIMEDIA
Q_DECLARE_METATYPE(QCameraDevice)
#endif

#if EB_QT5_MULTIMEDIA

CameraPage::CameraPage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent),
    camera(0),
    imageCapture(0),
    isCapturingImage(false)
{
    setTitleLabelText(tr("摄像头捕获测试"));
    showResolution = g_opt->cameraViewSize();

    initVideoDeviceBox();

    lockBtn = new QPushButton(tr("focus"), this);
    lockBtn->setObjectName("functionBtn_small");
    lockBtn->setFixedWidth(144);
    lockBtn->setEnabled(false);
    connect(lockBtn, SIGNAL(toggled(bool)), this, SLOT(toggleLock()));

    cameraViewfinder = new QCameraViewfinder(this);
    cameraViewfinder->setFixedSize(showResolution);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(videoDeviceBox);
    leftLayout->addWidget(lockBtn);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(20, 40, 20, 20);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(cameraViewfinder);
    this->setLayout(mainLayout);

    setCamera(QCameraInfo::defaultCamera());

    operationBar->secondButton()->setText(tr("Open"));
    operationBar->thirdButton()->setText(tr("Close"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(openCamera()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(closeCamera()));

#if LANGUAGE_CHINESE
    operationBar->secondButton()->setText(tr("打开"));
    operationBar->thirdButton()->setText(tr("关闭"));
#endif
}

void CameraPage::initVideoDeviceBox()
{
    videoDeviceBox = new QComboBox(this);
    videoDeviceBox->setFixedWidth(144);

    QActionGroup *videoDeviceGroup = new QActionGroup(this);
    videoDeviceGroup->setExclusive(true);

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameras) {
        qDebug() << QString("Camera >> %1, %2").arg(cameraInfo.deviceName()).arg(cameraInfo.description());

        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDeviceGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));

        videoDeviceBox->addItem(videoDeviceAction->text(), videoDeviceAction->data());

        if (cameraInfo == QCameraInfo::defaultCamera()) {
            videoDeviceAction->setChecked(true);
        }
    }

    connect(videoDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(videoDeviceBoxCurrentIndexChanged(int)));
}

void CameraPage::setCamera(const QCameraInfo &cameraInfo)
{
    qDebug() << QString("Select %1, %2").arg(cameraInfo.deviceName()).arg(cameraInfo.description());

    delete camera;
    delete imageCapture;

    camera = new QCamera(cameraInfo);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

    imageCapture = new QCameraImageCapture(camera);
    camera->setViewfinder(cameraViewfinder);

    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    QCameraViewfinderSettings set;
    set.setResolution(showResolution.width(), showResolution.height());
    camera->setViewfinderSettings(set);
    camera->setCaptureMode(QCamera::CaptureVideo);

    isCapturingImage = false;
}

void CameraPage::videoDeviceBoxCurrentIndexChanged(int index)
{
    setCamera(qvariant_cast<QCameraInfo>(videoDeviceBox->itemData(index, Qt::UserRole)));
}

void CameraPage::openCamera()
{
    camera->start();
    isCapturingImage = true;
    videoDeviceBox->setEnabled(false);
    lockBtn->setEnabled(true);
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(true);
}

void CameraPage::closeCamera()
{
    if (isCapturingImage) {
        camera->stop();
        videoDeviceBox->setEnabled(true);
        lockBtn->setEnabled(false);
        operationBar->thirdButton()->setEnabled(false);
        operationBar->secondButton()->setEnabled(true);
    }
}

void CameraPage::toggleLock()
{
    switch (camera->lockStatus()) {
    case QCamera::Searching:
    case QCamera::Locked:
        camera->unlock();
        break;
    case QCamera::Unlocked:
        camera->searchAndLock();
        break;
    default:
        break;
    }
}

void CameraPage::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}

void CameraPage::updateCameraState(QCamera::State)
{
}

void CameraPage::readyForCapture(bool ready)
{
    Q_UNUSED(ready);
}

void CameraPage::processCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);
    Q_UNUSED(img);
}

void CameraPage::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    Q_UNUSED(fileName);
}

void CameraPage::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;
}

#elif EB_QT6_MULTIMEDIA

CameraPage::CameraPage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent),
    camera(nullptr),
    captureSession(nullptr),
    videoWidget(nullptr),
    imageCapture(nullptr),
    isCapturingImage(false)
{
    setTitleLabelText(tr("摄像头捕获测试"));
    showResolution = g_opt->cameraViewSize();

    captureSession = new QMediaCaptureSession(this);

    initVideoDeviceBox();

    lockBtn = new QPushButton(tr("focus"), this);
    lockBtn->setObjectName("functionBtn_small");
    lockBtn->setFixedWidth(144);
    lockBtn->hide();

    videoWidget = new QVideoWidget(this);
    videoWidget->setFixedSize(showResolution);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(videoDeviceBox);
    leftLayout->addWidget(lockBtn);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(20, 40, 20, 20);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(videoWidget);
    setLayout(mainLayout);

    const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    if (!devices.isEmpty()) {
        setCameraDevice(devices.first());
    }

    operationBar->secondButton()->setText(tr("Open"));
    operationBar->thirdButton()->setText(tr("Close"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->secondButton(), &QPushButton::clicked, this, &CameraPage::openCamera);
    connect(operationBar->thirdButton(), &QPushButton::clicked, this, &CameraPage::closeCamera);

#if LANGUAGE_CHINESE
    operationBar->secondButton()->setText(tr("打开"));
    operationBar->thirdButton()->setText(tr("关闭"));
#endif
}

void CameraPage::initVideoDeviceBox()
{
    videoDeviceBox = new QComboBox(this);
    videoDeviceBox->setFixedWidth(144);

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &device : cameras) {
        qDebug() << "Camera >>" << device.id() << device.description();
        videoDeviceBox->addItem(device.description(), QVariant::fromValue(device));
    }

    connect(videoDeviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraPage::videoDeviceBoxCurrentIndexChanged);
}

void CameraPage::setCameraDevice(const QCameraDevice &device)
{
    delete camera;
    camera = new QCamera(device, this);

    connect(camera, &QCamera::errorOccurred, this, &CameraPage::displayCameraError);
    connect(camera, &QCamera::activeChanged, this, &CameraPage::updateCameraState);

    delete imageCapture;
    imageCapture = new QImageCapture(this);
    connect(imageCapture, &QImageCapture::errorOccurred,
            this, &CameraPage::displayCaptureError);

    captureSession->setCamera(camera);
    captureSession->setVideoOutput(videoWidget);
    captureSession->setImageCapture(imageCapture);

    isCapturingImage = false;
}

void CameraPage::videoDeviceBoxCurrentIndexChanged(int index)
{
    const QCameraDevice device = videoDeviceBox->itemData(index).value<QCameraDevice>();
    setCameraDevice(device);
}

void CameraPage::openCamera()
{
    if (!camera) {
        return;
    }
    camera->start();
    isCapturingImage = true;
    videoDeviceBox->setEnabled(false);
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(true);
}

void CameraPage::closeCamera()
{
    if (!camera || !isCapturingImage) {
        return;
    }
    camera->stop();
    isCapturingImage = false;
    videoDeviceBox->setEnabled(true);
    operationBar->thirdButton()->setEnabled(false);
    operationBar->secondButton()->setEnabled(true);
}

void CameraPage::displayCameraError(QCamera::Error error, const QString &errorString)
{
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Camera error"), errorString);
}

void CameraPage::updateCameraState(bool active)
{
    Q_UNUSED(active);
}

void CameraPage::displayCaptureError(int id, QImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;
}

#else

CameraPage::CameraPage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent),
    isCapturingImage(false)
{
    setTitleLabelText(tr("摄像头捕获测试"));
    showResolution = g_opt->cameraViewSize();

    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(false);
}

void CameraPage::initVideoDeviceBox()
{
}

#endif

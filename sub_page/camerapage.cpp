#include "camerapage.h"
#include <QTextCodec>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QAction>
#include <QVariant>

#if QT_VERSION_5
/* QVaiant 不能识别自定义类型和其他非 QMetaType 内置类型， 而 QCameraInfo 是非 QMetaType 内置类型，因此需要进行声明 */
Q_DECLARE_METATYPE(QCameraInfo)


CameraPage::CameraPage(EbOptions *options, QWidget *parent) :
    PageWidget(options, parent),
    camera(0),
    imageCapture(0),
    isCapturingImage(false)
#else
CameraPage::CameraPage(EbOptions *options, QWidget *parent) :
      PageWidget(options, parent)
#endif
{
    //setTitleLabelText(tr("Camera Capture Test"));
    setTitleLabelText(tr("摄像头捕获测试"));
    showResolution = g_opt->cameraViewSize();

#if QT_VERSION_5
    initVideoDeviceBox();

    lockBtn = new QPushButton(tr("focus"), this);
    lockBtn->setObjectName("functionBtn_small");
    lockBtn->setFixedWidth(144);
    lockBtn->setEnabled(false);
    connect(lockBtn, SIGNAL(toggled(bool)), this, SLOT(toggleLock()));

    cameraViewfinder = new QCameraViewfinder(this);
    //cameraViewfinder->setGeometry(130, 96+36, showResolution.width(), showResolution.height());
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

#endif

    /*
    processImage = new ProcessImage(this);
    processImage->setGeometry(130, 96+36, 640, 480);
    processImage->hide();

    displayArea = new QLabel(this);
    displayArea->setAlignment(Qt::AlignCenter);
    displayArea->setGeometry(130, 96+36, 640, 480);
    displayArea->setStyleSheet("background-color: Black; color: White;");
    displayArea->show();
    */

    //operationBar->firstButton()->setText(tr("Select"));
    operationBar->secondButton()->setText(tr("Open"));
    operationBar->thirdButton()->setText(tr("Close"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(openCamera()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(closeCamera()));

    /*
    if(CAMERA_NONE == processImage->cameraStatus || CAMERA_ERROR == processImage->cameraStatus) {
        processImage->hide();
        displayArea->show();
        displayArea->setText(tr("Camera not found or error."));
        operationBar->thirdButton()->setEnabled(false);
        operationBar->secondButton()->setEnabled(false);
    }
    */

#if LANGUAGE_CHINESE
    //operationBar->firstButton()->setText(tr("选择"));
    operationBar->secondButton()->setText(tr("打开"));
    operationBar->thirdButton()->setText(tr("关闭"));
#endif

}

#if QT_VERSION_5

void CameraPage::initVideoDeviceBox()
{
    videoDeviceBox = new QComboBox(this);
    videoDeviceBox->setFixedWidth(144);

    QActionGroup *videoDeviceGroup = new QActionGroup(this);
    videoDeviceGroup->setExclusive(true);

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameras) {
        qDebug() << QString("Camera >> %1, %2").arg(cameraInfo.deviceName()).arg(cameraInfo.description());

        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDeviceGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));

        videoDeviceBox->addItem(videoDeviceAction->text(), videoDeviceAction->data());

        if(cameraInfo == QCameraInfo::defaultCamera()) {
            videoDeviceAction->setChecked(true);
        }
    }

    connect(videoDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(videoDeviceBoxCurrentIndexChanged(int)));
}

void CameraPage::setCamera(const QCameraInfo &cameraInfo)
{
    qDebug() << QString("Select %1, %2").arg(cameraInfo.deviceName()).arg(cameraInfo.description());
#if 1
    delete camera;
    delete imageCapture;
#endif
    camera = new QCamera(cameraInfo);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

    imageCapture = new QCameraImageCapture(camera);
    camera->setViewfinder(cameraViewfinder);

    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    //imageCapture->setCaptureDestination(QCameraImageCapture::Capturet);
    //camera->setCaptureMode(QCamera::CaptureStillImage);

#ifdef QT_VERSION_5
    QCameraViewfinderSettings set;
    set.setResolution(showResolution.width(), showResolution.height());
    camera->setViewfinderSettings(set);
#endif
    camera->setCaptureMode(QCamera::CaptureVideo);

    isCapturingImage = false;
}


void CameraPage::videoDeviceBoxCurrentIndexChanged(int index)
{
    qDebug() << "videoDeviceBoxCurrentIndexChanged()";

    setCamera(qvariant_cast<QCameraInfo>(videoDeviceBox->itemData(index, Qt::UserRole)));
}


void CameraPage::openCamera()
{
    qDebug() << "Open Camera";

    /*
    if(CAMERA_EXISTED == processImage->cameraStatus || CAMERA_CLOSED == processImage->cameraStatus) {
        operationBar->secondButton()->setEnabled(false);
        operationBar->thirdButton()->setEnabled(true);
        if(0 == processImage->openCamera()) {
            displayArea->hide();
            processImage->show();
        }
        else {
            operationBar->secondButton()->setEnabled(true);
            operationBar->thirdButton()->setEnabled(false);
        }
    }
    */

    camera->start();
    isCapturingImage = true;
    videoDeviceBox->setEnabled(false);
    lockBtn->setEnabled(true);
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(true);
}

void CameraPage::closeCamera()
{
    qDebug() << "Close Camera";

    /*
    if(CAMERA_OPENED == processImage->cameraStatus) {
        operationBar->thirdButton()->setEnabled(false);
        operationBar->secondButton()->setEnabled(true);
        if(0 == processImage->closeCamera()) {
            processImage->hide();
            displayArea->show();
        }
        else {
            operationBar->thirdButton()->setEnabled(true);
            operationBar->secondButton()->setEnabled(false);
        }
    }
    */
    if(isCapturingImage) {
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

}

void CameraPage::processCapturedImage(int requestId, const QImage &img)
{

}

void CameraPage::imageSaved(int id, const QString &fileName)
{

}

void CameraPage::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;

}
#endif

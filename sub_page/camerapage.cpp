#include "camerapage.h"
#include <QTextCodec>
#include <QMessageBox>


CameraPage::CameraPage(QWidget *parent) :
    PageWidget(parent)
{
    //setTitleLabelText(tr("Camera Capture Test"));
    setTitleLabelText(tr("摄像头捕获测试"));

    showResolution.w = 640;
    showResolution.h = 480;

    cameraViewfinder = new QCameraViewfinder(this);
    cameraViewfinder->setGeometry(130, 96+36, showResolution.w, showResolution.h);

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

    operationBar->secondButton()->setText(tr("Open"));
    operationBar->thirdButton()->setText(tr("Close"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->firstButton()->setEnabled(false);
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
    operationBar->secondButton()->setText(tr("打开"));
    operationBar->thirdButton()->setText(tr("关闭"));
#endif

    setCamera(QCameraInfo::defaultCamera());
}


void CameraPage::setCamera(const QCameraInfo &cameraInfo)
{
    //delete imageCapture;
    //delete camera;

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

    QCameraViewfinderSettings set;
    set.setResolution(showResolution.w, showResolution.h);
    camera->setViewfinderSettings(set);
    camera->setCaptureMode(QCamera::CaptureVideo);

    isCapturingImage = false;
}


void CameraPage::openCamera()
{
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
    operationBar->secondButton()->setEnabled(false);
    operationBar->thirdButton()->setEnabled(true);
}

void CameraPage::closeCamera()
{
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
        operationBar->thirdButton()->setEnabled(false);
        operationBar->secondButton()->setEnabled(true);
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

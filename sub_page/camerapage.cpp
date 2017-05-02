#include "camerapage.h"
#include <QTextCodec>

CameraPage::CameraPage(QWidget *parent) :
    PageWidget(parent)
{
    setTitleLabelText(tr("Camera Capture Test"));

    processImage = new ProcessImage(this);
    processImage->setGeometry(130, 96+36, 640, 480);
    processImage->hide();

    displayArea = new QLabel(this);
    displayArea->setAlignment(Qt::AlignCenter);
    displayArea->setGeometry(130, 96+36, 640, 480);
    displayArea->setStyleSheet("background-color: Black; color: White;");
    displayArea->show();

    operationBar->secondButton()->setText(tr("Open"));
    operationBar->thirdButton()->setText(tr("Close"));
    operationBar->thirdButton()->setEnabled(false);
    operationBar->firstButton()->setEnabled(false);
    operationBar->fourthButton()->setEnabled(false);

    connect(operationBar->secondButton(), SIGNAL(clicked()), this, SLOT(openCamera()));
    connect(operationBar->thirdButton(), SIGNAL(clicked()), this, SLOT(closeCamera()));

    if(CAMERA_NONE == processImage->cameraStatus || CAMERA_ERROR == processImage->cameraStatus) {
        processImage->hide();
        displayArea->show();
        displayArea->setText(tr("Camera not found or error."));
        operationBar->thirdButton()->setEnabled(false);
        operationBar->secondButton()->setEnabled(false);
    }
}

void CameraPage::openCamera()
{
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
}

void CameraPage::closeCamera()
{
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
}


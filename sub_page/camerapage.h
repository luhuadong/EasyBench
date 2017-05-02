#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include "custom_widget/pagewidget.h"
#include "module/camera/processimage.h"

#include <QString>
#include <QWidget>
#include <QLabel>

class CameraPage : public PageWidget
{
    Q_OBJECT
public:
    explicit CameraPage(QWidget *parent = 0);

public slots:

private slots:
    void openCamera();
    void closeCamera();

private:
    ProcessImage *processImage;
    QLabel *displayArea;

};

#endif // CAMERAPAGE_H

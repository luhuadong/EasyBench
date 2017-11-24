#-------------------------------------------------
#
# Project created by QtCreator 2016-12-01T14:26:10
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gyt_box
TEMPLATE = app


SOURCES += main.cpp\
        gytboxwidget.cpp \
    gyt_common.c \
    custom_widget/operationbar.cpp \
    custom_widget/pagewidget.cpp \
    sub_page/lcdpage.cpp \
    sub_page/aboutpage.cpp \
    sub_page/camerapage.cpp \
    sub_page/datetimepage.cpp \
    sub_page/monitorpage.cpp \
    sub_page/realtimepage.cpp \
    sub_page/touchpage.cpp \
    sub_page/versionpage.cpp \
    module/camera/processimage.cpp \
    module/camera/videodevice.cpp \
    module/lcd/grayscalewidget.cpp \
    module/monitor/basepcbthread.cpp \
    module/monitor/cpustatthread.cpp \
    custom_widget/inputlineedit.cpp \
    sub_page/networkpage.cpp \
    sub_page/serialportpage.cpp

HEADERS  += gytboxwidget.h \
    gyt_common.h \
    custom_widget/operationbar.h \
    custom_widget/pagewidget.h \
    sub_page/lcdpage.h \
    sub_page/aboutpage.h \
    sub_page/camerapage.h \
    sub_page/datetimepage.h \
    sub_page/monitorpage.h \
    sub_page/realtimepage.h \
    sub_page/touchpage.h \
    sub_page/versionpage.h \
    module/camera/processimage.h \
    module/camera/videodevice.h \
    module/lcd/grayscalewidget.h \
    module/monitor/basepcbthread.h \
    module/monitor/cpustatthread.h \
    custom_widget/inputlineedit.h \
    sub_page/networkpage.h \
    sub_page/serialportpage.h

RESOURCES += \
    resource/images.qrc \
    resource/stylesheet.qrc

OTHER_FILES +=

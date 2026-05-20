#-------------------------------------------------
#
# Project created by QtCreator 2016-12-01T14:26:10
#
#-------------------------------------------------

QT += core gui
QT += network
QT += multimedia multimediawidgets
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = easybench
TEMPLATE = app

#LIBS    +=-L/opt/fsl-imx-x11/3.14.52-1.1.0/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/lib -lasound
#LIBS    +=-L/opt/poky/1.8.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/lib -lasound


SOURCES += main.cpp\
        mainwidget.cpp \
    eb_common.c \
    custom_widget/statusbar.cpp \
    custom_widget/pagewidget.cpp \
    sub_page/lcdpage.cpp \
    sub_page/camerapage.cpp \
    sub_page/datetimepage.cpp \
    sub_page/realtimepage.cpp \
    sub_page/serialpage.cpp \
    sub_page/storagepage.cpp \
    sub_page/versionpage.cpp \
    module/camera/processimage.cpp \
    module/camera/videodevice.cpp \
    module/lcd/grayscalewidget.cpp \
    module/monitor/basepcbthread.cpp \
    module/monitor/cpustatthread.cpp \
    module/monitor/eb_sysstats.cpp \
    module/monitor/eb_version_info.cpp \
    module/network/eb_net_util.cpp \
    module/update/eb_update_util.cpp \
    custom_widget/inputlineedit.cpp \
    sub_page/networkpage.cpp \
    sub_page/upgradepage.cpp \
    sub_page/systempage.cpp \
    sub_page/audiopage.cpp \
    module/audio/eb_pcm_util.cpp \
    module/audio/pcm_memory_device.cpp \
    eb_options.cpp \

HEADERS  += mainwidget.h \
    eb_common.h \
    custom_widget/statusbar.h \
    custom_widget/pagewidget.h \
    sub_page/lcdpage.h \
    sub_page/camerapage.h \
    sub_page/datetimepage.h \
    sub_page/realtimepage.h \
    sub_page/serialpage.h \
    sub_page/storagepage.h \
    sub_page/versionpage.h \
    module/camera/processimage.h \
    module/camera/videodevice.h \
    module/lcd/grayscalewidget.h \
    module/monitor/basepcbthread.h \
    module/monitor/cpustatthread.h \
    module/monitor/eb_sysstats.h \
    module/monitor/eb_version_info.h \
    module/network/eb_net_util.h \
    module/update/eb_update_util.h \
    custom_widget/inputlineedit.h \
    sub_page/networkpage.h \
    sub_page/upgradepage.h \
    sub_page/systempage.h \
    sub_page/audiopage.h \
    module/audio/eb_pcm_util.h \
    module/audio/pcm_memory_device.h \
    eb_options.h \

RESOURCES += \
    resource/images.qrc \
    resource/stylesheet.qrc

OTHER_FILES +=

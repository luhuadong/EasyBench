#############################################################################
# Makefile for building: gyt_box
# Generated by qmake (2.01a) (Qt 4.8.5) on: ?? 12? 19 03:03:20 2018
# Project:  gyt_box.pro
# Template: app
# Command: /opt/poky/1.5.3/sysroots/x86_64-pokysdk-linux/usr/bin/qmake -spec ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile gyt_box.pro
#############################################################################

####### Compiler, tools and options

CC            = $(OE_QMAKE_CC)
CXX           = $(OE_QMAKE_CXX)
DEFINES       = -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -pipe $(OE_QMAKE_CFLAGS) -DLINUX=1 -DEGL_API_FB=1 -g -g -Wall -W -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -pipe -pipe $(OE_QMAKE_CFLAGS) -DLINUX=1 -DEGL_API_FB=1 $(OE_QMAKE_CXXFLAGS) -g -g -g -Wall -W -Wall -W -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++ -I. -I$(OE_QMAKE_INCDIR_QT)/QtCore -I$(OE_QMAKE_INCDIR_QT)/QtNetwork -I$(OE_QMAKE_INCDIR_QT)/QtGui -I$(OE_QMAKE_INCDIR_QT) -I.
LINK          = $(OE_QMAKE_LINK)
LFLAGS        = $(OE_QMAKE_LDFLAGS) -Wl,-rpath-link,$(OE_QMAKE_LIBDIR_QT)
LIBS          = $(SUBLIBS)  -L$(OE_QMAKE_LIBDIR_QT) -lQtGui -lQtNetwork -lQtCore -lpthread 
AR            = $(OE_QMAKE_AR) cqs
RANLIB        = 
QMAKE         = /opt/poky/1.5.3/sysroots/x86_64-pokysdk-linux/usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = $(OE_QMAKE_STRIP)
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
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
		sub_page/serialportpage.cpp moc_gytboxwidget.cpp \
		moc_operationbar.cpp \
		moc_pagewidget.cpp \
		moc_lcdpage.cpp \
		moc_aboutpage.cpp \
		moc_camerapage.cpp \
		moc_datetimepage.cpp \
		moc_monitorpage.cpp \
		moc_realtimepage.cpp \
		moc_touchpage.cpp \
		moc_versionpage.cpp \
		moc_processimage.cpp \
		moc_videodevice.cpp \
		moc_grayscalewidget.cpp \
		moc_basepcbthread.cpp \
		moc_cpustatthread.cpp \
		moc_inputlineedit.cpp \
		moc_networkpage.cpp \
		moc_serialportpage.cpp \
		qrc_images.cpp \
		qrc_stylesheet.cpp
OBJECTS       = main.o \
		gytboxwidget.o \
		gyt_common.o \
		operationbar.o \
		pagewidget.o \
		lcdpage.o \
		aboutpage.o \
		camerapage.o \
		datetimepage.o \
		monitorpage.o \
		realtimepage.o \
		touchpage.o \
		versionpage.o \
		processimage.o \
		videodevice.o \
		grayscalewidget.o \
		basepcbthread.o \
		cpustatthread.o \
		inputlineedit.o \
		networkpage.o \
		serialportpage.o \
		moc_gytboxwidget.o \
		moc_operationbar.o \
		moc_pagewidget.o \
		moc_lcdpage.o \
		moc_aboutpage.o \
		moc_camerapage.o \
		moc_datetimepage.o \
		moc_monitorpage.o \
		moc_realtimepage.o \
		moc_touchpage.o \
		moc_versionpage.o \
		moc_processimage.o \
		moc_videodevice.o \
		moc_grayscalewidget.o \
		moc_basepcbthread.o \
		moc_cpustatthread.o \
		moc_inputlineedit.o \
		moc_networkpage.o \
		moc_serialportpage.o \
		qrc_images.o \
		qrc_stylesheet.o
DIST          = ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/linux.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/g++-unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/qconfig.pri \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_functions.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_config.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_pre.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/debug.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_post.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/shared.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/warn_on.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/thread.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/moc.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/resources.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/uic.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/yacc.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/lex.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		gyt_box.pro
QMAKE_TARGET  = gyt_box
DESTDIR       = 
TARGET        = gyt_box

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9]\+\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: gyt_box.pro  ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++/qmake.conf ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/linux.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/g++-unix.conf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/qconfig.pri \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_functions.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_config.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_pre.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/debug.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_post.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/shared.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/warn_on.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/thread.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/moc.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/resources.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/uic.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/yacc.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/lex.prf \
		../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile gyt_box.pro
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/unix.conf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/linux.conf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base.conf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/common/g++-unix.conf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/qconfig.pri:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_functions.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt_config.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_pre.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/debug.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/default_post.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/shared.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/warn_on.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/qt.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/unix/thread.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/moc.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/resources.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/uic.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/yacc.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/lex.prf:
../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec ../../../opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile gyt_box.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/gyt_box1.0.0 || $(MKDIR) .tmp/gyt_box1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/gyt_box1.0.0/ && $(COPY_FILE) --parents gytboxwidget.h gyt_common.h custom_widget/operationbar.h custom_widget/pagewidget.h sub_page/lcdpage.h sub_page/aboutpage.h sub_page/camerapage.h sub_page/datetimepage.h sub_page/monitorpage.h sub_page/realtimepage.h sub_page/touchpage.h sub_page/versionpage.h module/camera/processimage.h module/camera/videodevice.h module/lcd/grayscalewidget.h module/monitor/basepcbthread.h module/monitor/cpustatthread.h custom_widget/inputlineedit.h sub_page/networkpage.h sub_page/serialportpage.h .tmp/gyt_box1.0.0/ && $(COPY_FILE) --parents resource/images.qrc resource/stylesheet.qrc .tmp/gyt_box1.0.0/ && $(COPY_FILE) --parents main.cpp gytboxwidget.cpp gyt_common.c custom_widget/operationbar.cpp custom_widget/pagewidget.cpp sub_page/lcdpage.cpp sub_page/aboutpage.cpp sub_page/camerapage.cpp sub_page/datetimepage.cpp sub_page/monitorpage.cpp sub_page/realtimepage.cpp sub_page/touchpage.cpp sub_page/versionpage.cpp module/camera/processimage.cpp module/camera/videodevice.cpp module/lcd/grayscalewidget.cpp module/monitor/basepcbthread.cpp module/monitor/cpustatthread.cpp custom_widget/inputlineedit.cpp sub_page/networkpage.cpp sub_page/serialportpage.cpp .tmp/gyt_box1.0.0/ && (cd `dirname .tmp/gyt_box1.0.0` && $(TAR) gyt_box1.0.0.tar gyt_box1.0.0 && $(COMPRESS) gyt_box1.0.0.tar) && $(MOVE) `dirname .tmp/gyt_box1.0.0`/gyt_box1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/gyt_box1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_gytboxwidget.cpp moc_operationbar.cpp moc_pagewidget.cpp moc_lcdpage.cpp moc_aboutpage.cpp moc_camerapage.cpp moc_datetimepage.cpp moc_monitorpage.cpp moc_realtimepage.cpp moc_touchpage.cpp moc_versionpage.cpp moc_processimage.cpp moc_videodevice.cpp moc_grayscalewidget.cpp moc_basepcbthread.cpp moc_cpustatthread.cpp moc_inputlineedit.cpp moc_networkpage.cpp moc_serialportpage.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_gytboxwidget.cpp moc_operationbar.cpp moc_pagewidget.cpp moc_lcdpage.cpp moc_aboutpage.cpp moc_camerapage.cpp moc_datetimepage.cpp moc_monitorpage.cpp moc_realtimepage.cpp moc_touchpage.cpp moc_versionpage.cpp moc_processimage.cpp moc_videodevice.cpp moc_grayscalewidget.cpp moc_basepcbthread.cpp moc_cpustatthread.cpp moc_inputlineedit.cpp moc_networkpage.cpp moc_serialportpage.cpp
moc_gytboxwidget.cpp: sub_page/lcdpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/lcd/grayscalewidget.h \
		sub_page/touchpage.h \
		sub_page/camerapage.h \
		module/camera/processimage.h \
		module/camera/videodevice.h \
		sub_page/datetimepage.h \
		custom_widget/inputlineedit.h \
		sub_page/monitorpage.h \
		sub_page/versionpage.h \
		sub_page/realtimepage.h \
		sub_page/aboutpage.h \
		sub_page/networkpage.h \
		sub_page/serialportpage.h \
		gytboxwidget.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) gytboxwidget.h -o moc_gytboxwidget.cpp

moc_operationbar.cpp: custom_widget/operationbar.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) custom_widget/operationbar.h -o moc_operationbar.cpp

moc_pagewidget.cpp: custom_widget/operationbar.h \
		gyt_common.h \
		custom_widget/pagewidget.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) custom_widget/pagewidget.h -o moc_pagewidget.cpp

moc_lcdpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/lcd/grayscalewidget.h \
		sub_page/lcdpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/lcdpage.h -o moc_lcdpage.cpp

moc_aboutpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/aboutpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/aboutpage.h -o moc_aboutpage.cpp

moc_camerapage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/camera/processimage.h \
		module/camera/videodevice.h \
		sub_page/camerapage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/camerapage.h -o moc_camerapage.cpp

moc_datetimepage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		custom_widget/inputlineedit.h \
		sub_page/datetimepage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/datetimepage.h -o moc_datetimepage.cpp

moc_monitorpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/monitorpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/monitorpage.h -o moc_monitorpage.cpp

moc_realtimepage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/realtimepage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/realtimepage.h -o moc_realtimepage.cpp

moc_touchpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/touchpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/touchpage.h -o moc_touchpage.cpp

moc_versionpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/versionpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/versionpage.h -o moc_versionpage.cpp

moc_processimage.cpp: module/camera/videodevice.h \
		module/camera/processimage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) module/camera/processimage.h -o moc_processimage.cpp

moc_videodevice.cpp: module/camera/videodevice.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) module/camera/videodevice.h -o moc_videodevice.cpp

moc_grayscalewidget.cpp: module/lcd/grayscalewidget.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) module/lcd/grayscalewidget.h -o moc_grayscalewidget.cpp

moc_basepcbthread.cpp: gyt_common.h \
		module/monitor/basepcbthread.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) module/monitor/basepcbthread.h -o moc_basepcbthread.cpp

moc_cpustatthread.cpp: module/monitor/cpustatthread.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) module/monitor/cpustatthread.h -o moc_cpustatthread.cpp

moc_inputlineedit.cpp: custom_widget/inputlineedit.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) custom_widget/inputlineedit.h -o moc_inputlineedit.cpp

moc_networkpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/networkpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/networkpage.h -o moc_networkpage.cpp

moc_serialportpage.cpp: custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		sub_page/serialportpage.h
	$(OE_QMAKE_MOC) $(DEFINES) $(INCPATH) sub_page/serialportpage.h -o moc_serialportpage.cpp

compiler_rcc_make_all: qrc_images.cpp qrc_stylesheet.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_images.cpp qrc_stylesheet.cpp
qrc_images.cpp: resource/images.qrc
	$(OE_QMAKE_RCC) -name images resource/images.qrc -o qrc_images.cpp

qrc_stylesheet.cpp: resource/stylesheet.qrc
	$(OE_QMAKE_RCC) -name stylesheet resource/stylesheet.qrc -o qrc_stylesheet.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean 

####### Compile

main.o: main.cpp gytboxwidget.h \
		sub_page/lcdpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/lcd/grayscalewidget.h \
		sub_page/touchpage.h \
		sub_page/camerapage.h \
		module/camera/processimage.h \
		module/camera/videodevice.h \
		sub_page/datetimepage.h \
		custom_widget/inputlineedit.h \
		sub_page/monitorpage.h \
		sub_page/versionpage.h \
		sub_page/realtimepage.h \
		sub_page/aboutpage.h \
		sub_page/networkpage.h \
		sub_page/serialportpage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

gytboxwidget.o: gytboxwidget.cpp gytboxwidget.h \
		sub_page/lcdpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/lcd/grayscalewidget.h \
		sub_page/touchpage.h \
		sub_page/camerapage.h \
		module/camera/processimage.h \
		module/camera/videodevice.h \
		sub_page/datetimepage.h \
		custom_widget/inputlineedit.h \
		sub_page/monitorpage.h \
		sub_page/versionpage.h \
		sub_page/realtimepage.h \
		sub_page/aboutpage.h \
		sub_page/networkpage.h \
		sub_page/serialportpage.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gytboxwidget.o gytboxwidget.cpp

gyt_common.o: gyt_common.c gyt_common.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o gyt_common.o gyt_common.c

operationbar.o: custom_widget/operationbar.cpp custom_widget/operationbar.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o operationbar.o custom_widget/operationbar.cpp

pagewidget.o: custom_widget/pagewidget.cpp custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o pagewidget.o custom_widget/pagewidget.cpp

lcdpage.o: sub_page/lcdpage.cpp sub_page/lcdpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/lcd/grayscalewidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o lcdpage.o sub_page/lcdpage.cpp

aboutpage.o: sub_page/aboutpage.cpp sub_page/aboutpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o aboutpage.o sub_page/aboutpage.cpp

camerapage.o: sub_page/camerapage.cpp sub_page/camerapage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/camera/processimage.h \
		module/camera/videodevice.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o camerapage.o sub_page/camerapage.cpp

datetimepage.o: sub_page/datetimepage.cpp sub_page/datetimepage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		custom_widget/inputlineedit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o datetimepage.o sub_page/datetimepage.cpp

monitorpage.o: sub_page/monitorpage.cpp sub_page/monitorpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h \
		module/monitor/basepcbthread.h \
		module/monitor/cpustatthread.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o monitorpage.o sub_page/monitorpage.cpp

realtimepage.o: sub_page/realtimepage.cpp sub_page/realtimepage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o realtimepage.o sub_page/realtimepage.cpp

touchpage.o: sub_page/touchpage.cpp sub_page/touchpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o touchpage.o sub_page/touchpage.cpp

versionpage.o: sub_page/versionpage.cpp sub_page/versionpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o versionpage.o sub_page/versionpage.cpp

processimage.o: module/camera/processimage.cpp module/camera/processimage.h \
		module/camera/videodevice.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o processimage.o module/camera/processimage.cpp

videodevice.o: module/camera/videodevice.cpp module/camera/videodevice.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o videodevice.o module/camera/videodevice.cpp

grayscalewidget.o: module/lcd/grayscalewidget.cpp module/lcd/grayscalewidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o grayscalewidget.o module/lcd/grayscalewidget.cpp

basepcbthread.o: module/monitor/basepcbthread.cpp module/monitor/basepcbthread.h \
		gyt_common.h \
		sub_page/monitorpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o basepcbthread.o module/monitor/basepcbthread.cpp

cpustatthread.o: module/monitor/cpustatthread.cpp module/monitor/cpustatthread.h \
		sub_page/monitorpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cpustatthread.o module/monitor/cpustatthread.cpp

inputlineedit.o: custom_widget/inputlineedit.cpp custom_widget/inputlineedit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o inputlineedit.o custom_widget/inputlineedit.cpp

networkpage.o: sub_page/networkpage.cpp sub_page/networkpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o networkpage.o sub_page/networkpage.cpp

serialportpage.o: sub_page/serialportpage.cpp sub_page/serialportpage.h \
		custom_widget/pagewidget.h \
		custom_widget/operationbar.h \
		gyt_common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o serialportpage.o sub_page/serialportpage.cpp

moc_gytboxwidget.o: moc_gytboxwidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_gytboxwidget.o moc_gytboxwidget.cpp

moc_operationbar.o: moc_operationbar.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_operationbar.o moc_operationbar.cpp

moc_pagewidget.o: moc_pagewidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_pagewidget.o moc_pagewidget.cpp

moc_lcdpage.o: moc_lcdpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_lcdpage.o moc_lcdpage.cpp

moc_aboutpage.o: moc_aboutpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_aboutpage.o moc_aboutpage.cpp

moc_camerapage.o: moc_camerapage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_camerapage.o moc_camerapage.cpp

moc_datetimepage.o: moc_datetimepage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_datetimepage.o moc_datetimepage.cpp

moc_monitorpage.o: moc_monitorpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_monitorpage.o moc_monitorpage.cpp

moc_realtimepage.o: moc_realtimepage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_realtimepage.o moc_realtimepage.cpp

moc_touchpage.o: moc_touchpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_touchpage.o moc_touchpage.cpp

moc_versionpage.o: moc_versionpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_versionpage.o moc_versionpage.cpp

moc_processimage.o: moc_processimage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_processimage.o moc_processimage.cpp

moc_videodevice.o: moc_videodevice.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_videodevice.o moc_videodevice.cpp

moc_grayscalewidget.o: moc_grayscalewidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_grayscalewidget.o moc_grayscalewidget.cpp

moc_basepcbthread.o: moc_basepcbthread.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_basepcbthread.o moc_basepcbthread.cpp

moc_cpustatthread.o: moc_cpustatthread.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_cpustatthread.o moc_cpustatthread.cpp

moc_inputlineedit.o: moc_inputlineedit.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_inputlineedit.o moc_inputlineedit.cpp

moc_networkpage.o: moc_networkpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_networkpage.o moc_networkpage.cpp

moc_serialportpage.o: moc_serialportpage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_serialportpage.o moc_serialportpage.cpp

qrc_images.o: qrc_images.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_images.o qrc_images.cpp

qrc_stylesheet.o: qrc_stylesheet.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_stylesheet.o qrc_stylesheet.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:


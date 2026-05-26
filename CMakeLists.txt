cmake_minimum_required(VERSION 3.16)

project(TuxiBit VERSION 0.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(TB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

option(TUXIBIT_FORCE_QT6 "Force Qt 6 even when Qt 5 is available" OFF)
option(TUXIBIT_FORCE_QT5 "Force Qt 5 even when Qt 6 is available" OFF)

if(TUXIBIT_FORCE_QT6 AND TUXIBIT_FORCE_QT5)
    message(FATAL_ERROR "TUXIBIT_FORCE_QT6 and TUXIBIT_FORCE_QT5 cannot both be ON.")
endif()

set(_TB_QT_COMPONENTS Core Gui Widgets Network Xml Multimedia MultimediaWidgets Svg)

if(TUXIBIT_FORCE_QT5)
    find_package(Qt5 5.12 REQUIRED COMPONENTS ${_TB_QT_COMPONENTS})
    set(TB_QT_VERSION_MAJOR 5)
elseif(TUXIBIT_FORCE_QT6)
    find_package(Qt6 6.2 REQUIRED COMPONENTS ${_TB_QT_COMPONENTS} Core5Compat)
    set(TB_QT_VERSION_MAJOR 6)
else()
    find_package(Qt6 6.2 QUIET COMPONENTS ${_TB_QT_COMPONENTS} Core5Compat)
    if(Qt6_FOUND)
        set(TB_QT_VERSION_MAJOR 6)
    else()
        find_package(Qt5 5.12 REQUIRED COMPONENTS ${_TB_QT_COMPONENTS})
        set(TB_QT_VERSION_MAJOR 5)
    endif()
endif()

message(STATUS "TuxiBit: building with Qt ${TB_QT_VERSION_MAJOR}")

set(TUXIBIT_SOURCES
    src/main.cpp
    src/app/mainwidget.cpp
    src/app/tb_common.c
    src/app/tb_options.cpp
    src/app/tb_startup.cpp
    src/app/tb_branding.cpp
    src/app/tb_paths.cpp
    src/app/tb_thread_util.cpp
    src/widgets/statusbar.cpp
    src/widgets/pagewidget.cpp
    src/widgets/inputlineedit.cpp
    src/widgets/tb_widget_util.cpp
    src/pages/lcdpage.cpp
    src/pages/camerapage.cpp
    src/pages/datetimepage.cpp
    src/pages/realtimepage.cpp
    src/pages/serialpage.cpp
    src/pages/storagepage.cpp
    src/pages/versionpage.cpp
    src/pages/upgradepage.cpp
    src/pages/networkpage.cpp
    src/pages/systempage.cpp
    src/pages/audiopage.cpp
    src/modules/audio/tb_pcm_util.cpp
    src/modules/audio/pcm_memory_device.cpp
    src/modules/camera/processimage.cpp
    src/modules/camera/videodevice.cpp
    src/modules/lcd/grayscalewidget.cpp
    src/modules/monitor/basepcbthread.cpp
    src/modules/monitor/cpustatthread.cpp
    src/modules/monitor/tb_sysstats.cpp
    src/modules/monitor/tb_version_info.cpp
    src/modules/network/tb_net_util.cpp
    src/modules/update/tb_update_util.cpp
)

set(TUXIBIT_HEADERS
    src/app/mainwidget.h
    src/app/tb_common.h
    src/app/tb_options.h
    src/app/tb_startup.h
    src/app/tb_branding.h
    src/app/tb_paths.h
    src/app/tb_thread_util.h
    src/app/tb_qt_compat.h
    src/widgets/statusbar.h
    src/widgets/pagewidget.h
    src/widgets/inputlineedit.h
    src/widgets/tb_widget_util.h
    src/pages/lcdpage.h
    src/pages/camerapage.h
    src/pages/datetimepage.h
    src/pages/realtimepage.h
    src/pages/serialpage.h
    src/pages/storagepage.h
    src/pages/versionpage.h
    src/pages/upgradepage.h
    src/pages/networkpage.h
    src/pages/systempage.h
    src/pages/audiopage.h
    src/modules/audio/tb_pcm_util.h
    src/modules/audio/pcm_memory_device.h
    src/modules/camera/processimage.h
    src/modules/camera/videodevice.h
    src/modules/lcd/grayscalewidget.h
    src/modules/monitor/basepcbthread.h
    src/modules/monitor/cpustatthread.h
    src/modules/monitor/tb_sysstats.h
    src/modules/monitor/tb_version_info.h
    src/modules/network/tb_net_util.h
    src/modules/update/tb_update_util.h
)

set(TUXIBIT_RESOURCES
    resource/images.qrc
    resource/stylesheet.qrc
    resource/sounds.qrc
)

add_executable(tuxibit
    ${TUXIBIT_SOURCES}
    ${TUXIBIT_HEADERS}
    ${TUXIBIT_RESOURCES}
)

target_include_directories(tuxibit PRIVATE
    ${TB_SOURCE_DIR}
    ${TB_SOURCE_DIR}/app
    ${TB_SOURCE_DIR}/widgets
    ${TB_SOURCE_DIR}/pages
    ${TB_SOURCE_DIR}/modules/camera
    ${TB_SOURCE_DIR}/modules/lcd
    ${TB_SOURCE_DIR}/modules/monitor
    ${TB_SOURCE_DIR}/modules/audio
    ${TB_SOURCE_DIR}/modules/network
    ${TB_SOURCE_DIR}/modules/update
)

target_compile_definitions(tuxibit PRIVATE
    TB_QT_VERSION_MAJOR=${TB_QT_VERSION_MAJOR}
    TB_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    TB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    TB_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    TB_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if(TB_QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(tuxibit PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Xml
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Svg
        Qt6::Core5Compat
    )
else()
    target_link_libraries(tuxibit PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::Xml
        Qt5::Multimedia
        Qt5::MultimediaWidgets
        Qt5::Svg
    )
endif()

find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
target_include_directories(tuxibit PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(tuxibit PRIVATE Threads::Threads ${ALSA_LIBRARIES})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/TuxiBitInstall.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/TuxiBitPackaging.cmake)

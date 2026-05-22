cmake_minimum_required(VERSION 3.16)

project(EasyBench VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(EB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

option(EASYBENCH_FORCE_QT6 "Force Qt 6 even when Qt 5 is available" OFF)
option(EASYBENCH_FORCE_QT5 "Force Qt 5 even when Qt 6 is available" OFF)

if(EASYBENCH_FORCE_QT6 AND EASYBENCH_FORCE_QT5)
    message(FATAL_ERROR "EASYBENCH_FORCE_QT6 and EASYBENCH_FORCE_QT5 cannot both be ON.")
endif()

set(_EB_QT_COMPONENTS Core Gui Widgets Network Xml Multimedia MultimediaWidgets Svg)

if(EASYBENCH_FORCE_QT5)
    find_package(Qt5 5.12 REQUIRED COMPONENTS ${_EB_QT_COMPONENTS})
    set(EB_QT_VERSION_MAJOR 5)
elseif(EASYBENCH_FORCE_QT6)
    find_package(Qt6 6.2 REQUIRED COMPONENTS ${_EB_QT_COMPONENTS} Core5Compat)
    set(EB_QT_VERSION_MAJOR 6)
else()
    find_package(Qt6 6.2 QUIET COMPONENTS ${_EB_QT_COMPONENTS} Core5Compat)
    if(Qt6_FOUND)
        set(EB_QT_VERSION_MAJOR 6)
    else()
        find_package(Qt5 5.12 REQUIRED COMPONENTS ${_EB_QT_COMPONENTS})
        set(EB_QT_VERSION_MAJOR 5)
    endif()
endif()

message(STATUS "EasyBench: building with Qt ${EB_QT_VERSION_MAJOR}")

set(EASYBENCH_SOURCES
    src/main.cpp
    src/app/mainwidget.cpp
    src/app/eb_common.c
    src/app/eb_options.cpp
    src/app/eb_startup.cpp
    src/app/eb_branding.cpp
    src/app/eb_paths.cpp
    src/app/eb_thread_util.cpp
    src/widgets/statusbar.cpp
    src/widgets/pagewidget.cpp
    src/widgets/inputlineedit.cpp
    src/widgets/eb_widget_util.cpp
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
    src/modules/audio/eb_pcm_util.cpp
    src/modules/audio/pcm_memory_device.cpp
    src/modules/camera/processimage.cpp
    src/modules/camera/videodevice.cpp
    src/modules/lcd/grayscalewidget.cpp
    src/modules/monitor/basepcbthread.cpp
    src/modules/monitor/cpustatthread.cpp
    src/modules/monitor/eb_sysstats.cpp
    src/modules/monitor/eb_version_info.cpp
    src/modules/network/eb_net_util.cpp
    src/modules/update/eb_update_util.cpp
)

set(EASYBENCH_HEADERS
    src/app/mainwidget.h
    src/app/eb_common.h
    src/app/eb_options.h
    src/app/eb_startup.h
    src/app/eb_branding.h
    src/app/eb_paths.h
    src/app/eb_thread_util.h
    src/app/eb_qt_compat.h
    src/widgets/statusbar.h
    src/widgets/pagewidget.h
    src/widgets/inputlineedit.h
    src/widgets/eb_widget_util.h
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
    src/modules/audio/eb_pcm_util.h
    src/modules/audio/pcm_memory_device.h
    src/modules/camera/processimage.h
    src/modules/camera/videodevice.h
    src/modules/lcd/grayscalewidget.h
    src/modules/monitor/basepcbthread.h
    src/modules/monitor/cpustatthread.h
    src/modules/monitor/eb_sysstats.h
    src/modules/monitor/eb_version_info.h
    src/modules/network/eb_net_util.h
    src/modules/update/eb_update_util.h
)

set(EASYBENCH_RESOURCES
    resource/images.qrc
    resource/stylesheet.qrc
    resource/sounds.qrc
)

add_executable(easybench
    ${EASYBENCH_SOURCES}
    ${EASYBENCH_HEADERS}
    ${EASYBENCH_RESOURCES}
)

target_include_directories(easybench PRIVATE
    ${EB_SOURCE_DIR}
    ${EB_SOURCE_DIR}/app
    ${EB_SOURCE_DIR}/widgets
    ${EB_SOURCE_DIR}/pages
    ${EB_SOURCE_DIR}/modules/camera
    ${EB_SOURCE_DIR}/modules/lcd
    ${EB_SOURCE_DIR}/modules/monitor
    ${EB_SOURCE_DIR}/modules/audio
    ${EB_SOURCE_DIR}/modules/network
    ${EB_SOURCE_DIR}/modules/update
)

target_compile_definitions(easybench PRIVATE
    EB_QT_VERSION_MAJOR=${EB_QT_VERSION_MAJOR}
    EB_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    EB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    EB_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    EB_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if(EB_QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(easybench PRIVATE
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
    target_link_libraries(easybench PRIVATE
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
target_include_directories(easybench PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(easybench PRIVATE Threads::Threads ${ALSA_LIBRARIES})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/EasyBenchInstall.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/EasyBenchPackaging.cmake)

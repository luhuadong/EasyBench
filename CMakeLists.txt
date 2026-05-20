cmake_minimum_required(VERSION 3.16)

project(EasyBench VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
    main.cpp
    mainwidget.cpp
    eb_common.c
    eb_options.cpp
    eb_startup.cpp
    eb_branding.cpp
    custom_widget/statusbar.cpp
    custom_widget/pagewidget.cpp
    custom_widget/inputlineedit.cpp
    sub_page/lcdpage.cpp
    sub_page/camerapage.cpp
    sub_page/datetimepage.cpp
    sub_page/realtimepage.cpp
    sub_page/serialpage.cpp
    sub_page/storagepage.cpp
    sub_page/versionpage.cpp
    sub_page/upgradepage.cpp
    sub_page/networkpage.cpp
    sub_page/systempage.cpp
    sub_page/audiopage.cpp
    module/audio/eb_pcm_util.cpp
    module/audio/pcm_memory_device.cpp
    module/camera/processimage.cpp
    module/camera/videodevice.cpp
    module/lcd/grayscalewidget.cpp
    module/monitor/basepcbthread.cpp
    module/monitor/cpustatthread.cpp
    module/monitor/eb_sysstats.cpp
    module/monitor/eb_version_info.cpp
    module/network/eb_net_util.cpp
    module/update/eb_update_util.cpp
)

set(EASYBENCH_HEADERS
    mainwidget.h
    eb_common.h
    eb_options.h
    eb_startup.h
    eb_branding.h
    custom_widget/statusbar.h
    custom_widget/pagewidget.h
    custom_widget/inputlineedit.h
    sub_page/lcdpage.h
    sub_page/camerapage.h
    sub_page/datetimepage.h
    sub_page/realtimepage.h
    sub_page/serialpage.h
    sub_page/storagepage.h
    sub_page/versionpage.h
    sub_page/upgradepage.h
    sub_page/networkpage.h
    sub_page/systempage.h
    sub_page/audiopage.h
    module/audio/eb_pcm_util.h
    module/audio/pcm_memory_device.h
    module/camera/processimage.h
    module/camera/videodevice.h
    module/lcd/grayscalewidget.h
    module/monitor/basepcbthread.h
    module/monitor/cpustatthread.h
    module/monitor/eb_sysstats.h
    module/monitor/eb_version_info.h
    module/network/eb_net_util.h
    module/update/eb_update_util.h
)

set(EASYBENCH_RESOURCES
    resource/images.qrc
    resource/stylesheet.qrc
)

add_executable(easybench
    ${EASYBENCH_SOURCES}
    ${EASYBENCH_HEADERS}
    ${EASYBENCH_RESOURCES}
)

target_include_directories(easybench PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widget
    ${CMAKE_CURRENT_SOURCE_DIR}/sub_page
    ${CMAKE_CURRENT_SOURCE_DIR}/module/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/module/lcd
    ${CMAKE_CURRENT_SOURCE_DIR}/module/monitor
    ${CMAKE_CURRENT_SOURCE_DIR}/module/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/module/network
    ${CMAKE_CURRENT_SOURCE_DIR}/module/update
)

target_compile_definitions(easybench PRIVATE
    EB_QT_VERSION_MAJOR=${EB_QT_VERSION_MAJOR}
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

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
target_include_directories(easybench PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(easybench PRIVATE ${ALSA_LIBRARIES})

include(GNUInstallDirs)

install(TARGETS easybench RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Freedesktop: launcher + icon theme (GNOME/KDE/Ubuntu dock, etc.)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/easybench.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/easybench.desktop
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/easybench.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

set(_EB_ICON_SIZES 16 32 48 64 128 256 512)
foreach(_size IN LISTS _EB_ICON_SIZES)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps
        RENAME easybench.png
    )
endforeach()

# Legacy path still used by some installers and older desktop environments
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
    RENAME easybench.png
)

# Optional: refresh GTK icon cache on Ubuntu/Debian when available
install(CODE "
    find_program(_EB_UPDATE_ICON_CACHE gtk-update-icon-cache)
    if(_EB_UPDATE_ICON_CACHE)
        execute_process(
            COMMAND \${_EB_UPDATE_ICON_CACHE} -f -t
                \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/icons/hicolor\"
            RESULT_VARIABLE _EB_ICON_CACHE_RESULT
        )
    endif()
")

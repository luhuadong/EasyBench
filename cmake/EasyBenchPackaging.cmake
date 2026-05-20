# EasyBenchPackaging.cmake — CPack settings (include after EasyBenchInstall.cmake)

set(CPACK_PACKAGE_NAME "easybench")
set(CPACK_PACKAGE_VENDOR "EasyBench")
set(CPACK_PACKAGE_CONTACT "easybench@localhost")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux hardware diagnostics and system utilities")
set(CPACK_PACKAGE_DESCRIPTION
    "EasyBench: hardware diagnostics, network tools, and system monitoring for Linux devices.")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "easybench")
set(CPACK_SET_DESTDIR ON)
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_STRIP_FILES TRUE)

# TGZ for generic Linux; DEB for Debian/Ubuntu (optional)
set(CPACK_GENERATOR "TGZ")
if(UNIX AND NOT APPLE)
    list(APPEND CPACK_GENERATOR "DEB")
endif()

set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# Runtime deps vary by Qt major version; keep soft deps for desktop use
if(EB_QT_VERSION_MAJOR EQUAL 6)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libqt6widgets6, libqt6multimedia6, libqt6network6, libqt6svg6, libasound2, libc6")
else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libqt5widgets5, libqt5multimedia5, libqt5network5, libqt5svg5, libasound2, libc6")
endif()

include(CPack)

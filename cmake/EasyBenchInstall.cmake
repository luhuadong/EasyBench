# EasyBenchInstall.cmake — install rules (include after target `easybench` exists)

include(GNUInstallDirs)

# FHS: site configuration under /etc when installing to /usr
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(EB_INSTALL_SYSCONF_DIR "/etc")
else()
    set(EB_INSTALL_SYSCONF_DIR "${CMAKE_INSTALL_PREFIX}/etc")
endif()

# Desktop launcher (absolute Exec path for non-/usr/bin installs)
set(EB_DESKTOP_EXEC "${CMAKE_INSTALL_FULL_BINDIR}/easybench")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/easybench.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/easybench.desktop
    @ONLY
)

install(TARGETS easybench RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/easybench.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# Freedesktop icon theme + legacy pixmaps path
set(_EB_ICON_SIZES 16 32 48 64 128 256 512)
foreach(_size IN LISTS _EB_ICON_SIZES)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps
        RENAME easybench.png
    )
endforeach()

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
    RENAME easybench.png
)

# Configuration template (do not overwrite an existing site config)
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/easybench.conf
    DESTINATION ${EB_INSTALL_SYSCONF_DIR}/easybench
    RENAME easybench.conf.example
)

install(CODE "
    if(DEFINED ENV{DESTDIR})
        set(_eb_root \"\$ENV{DESTDIR}\")
    else()
        set(_eb_root \"\")
    endif()
    set(_eb_conf_dir \"\${_eb_root}${EB_INSTALL_SYSCONF_DIR}/easybench\")
    set(_eb_conf \"\${_eb_conf_dir}/easybench.conf\")
    file(MAKE_DIRECTORY \"\${_eb_conf_dir}\")
    if(NOT EXISTS \"\${_eb_conf}\")
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E copy
                \"${CMAKE_CURRENT_SOURCE_DIR}/config/easybench.conf\"
                \"\${_eb_conf}\"
        )
    endif()
")

# Legacy /etc/gbox symlink (staging/CPack only; real install: use scripts/cmake-install.sh)
install(CODE "
    if(NOT DEFINED ENV{DESTDIR} OR \"\$ENV{DESTDIR}\" STREQUAL \"\")
        message(STATUS \"EasyBench: skip /etc/gbox symlink (non-staging install)\")
    else()
        set(_eb_root \"\$ENV{DESTDIR}\")
        set(_eb_legacy \"\${_eb_root}/etc/gbox\")
        set(_eb_target \"\${_eb_root}${EB_INSTALL_SYSCONF_DIR}/easybench/easybench.conf\")
        file(MAKE_DIRECTORY \"\${_eb_legacy}\")
        if(EXISTS \"\${_eb_target}\" AND NOT EXISTS \"\${_eb_legacy}/easybench.conf\")
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
                    \"../easybench/easybench.conf\"
                    \"\${_eb_legacy}/easybench.conf\"
            )
        endif()
    endif()
")

# Deploy assets (I210 production tools)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deploy/bin/eepromARMtool")
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/deploy/bin/eepromARMtool
        DESTINATION ${CMAKE_INSTALL_DATADIR}/easybench/deploy/bin
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deploy/data/I210NIC-origin.otp")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/deploy/data/I210NIC-origin.otp
        DESTINATION ${CMAKE_INSTALL_DATADIR}/easybench/deploy/data
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resource/fonts/LiHeiPro.ttf")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/fonts/LiHeiPro.ttf
        DESTINATION ${CMAKE_INSTALL_DATADIR}/easybench/fonts
    )
endif()

# Optional: version info template
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config/easybench-version.conf.example")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/easybench-version.conf.example
        DESTINATION ${CMAKE_INSTALL_DATADIR}/easybench
    )
endif()

# Post-install: refresh icon cache & desktop database when installing on target system
install(CODE "
    if(NOT \"\$ENV{DESTDIR}\")
        find_program(_EB_GTK_ICON_CACHE gtk-update-icon-cache)
        if(_EB_GTK_ICON_CACHE)
            execute_process(
                COMMAND \${_EB_GTK_ICON_CACHE} -f -t
                    \"${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor\"
                RESULT_VARIABLE _eb_icon_cache_result
                ERROR_QUIET
            )
        endif()
        find_program(_EB_UPDATE_DESKTOP_DB update-desktop-database)
        if(_EB_UPDATE_DESKTOP_DB)
            execute_process(
                COMMAND \${_EB_UPDATE_DESKTOP_DB}
                    \"${CMAKE_INSTALL_FULL_DATADIR}/applications\"
                RESULT_VARIABLE _eb_desktop_db_result
                ERROR_QUIET
            )
        endif()
    endif()
")

# TuxiBitInstall.cmake — install rules (include after target `tuxibit` exists)

include(GNUInstallDirs)

# FHS: site configuration under /etc when installing to /usr
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(TB_INSTALL_SYSCONF_DIR "/etc")
else()
    set(TB_INSTALL_SYSCONF_DIR "${CMAKE_INSTALL_PREFIX}/etc")
endif()

# Desktop launcher (absolute Exec path for non-/usr/bin installs)
set(TB_DESKTOP_EXEC "${CMAKE_INSTALL_FULL_BINDIR}/tuxibit")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tuxibit.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/tuxibit.desktop
    @ONLY
)

install(TARGETS tuxibit RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/tuxibit.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# Freedesktop icon theme + legacy pixmaps path
set(_TB_ICON_SIZES 16 32 48 64 128 256 512)
foreach(_size IN LISTS _TB_ICON_SIZES)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps
        RENAME tuxibit.png
    )
endforeach()

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/logo.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
    RENAME tuxibit.png
)

# Configuration template (do not overwrite an existing site config)
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/tuxibit.conf
    DESTINATION ${TB_INSTALL_SYSCONF_DIR}/tuxibit
    RENAME tuxibit.conf.example
)

install(CODE "
    if(DEFINED ENV{DESTDIR})
        set(_tb_root \"\$ENV{DESTDIR}\")
    else()
        set(_tb_root \"\")
    endif()
    set(_tb_conf_dir \"\${_tb_root}${TB_INSTALL_SYSCONF_DIR}/tuxibit\")
    set(_tb_conf \"\${_tb_conf_dir}/tuxibit.conf\")
    file(MAKE_DIRECTORY \"\${_tb_conf_dir}\")
    if(NOT EXISTS \"\${_tb_conf}\")
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E copy
                \"${CMAKE_CURRENT_SOURCE_DIR}/config/tuxibit.conf\"
                \"\${_tb_conf}\"
        )
    endif()
")

# Legacy /etc/gbox symlink (staging/CPack only; real install: use scripts/cmake-install.sh)
install(CODE "
    if(NOT DEFINED ENV{DESTDIR} OR \"\$ENV{DESTDIR}\" STREQUAL \"\")
        message(STATUS \"TuxiBit: skip /etc/gbox symlink (non-staging install)\")
    else()
        set(_tb_root \"\$ENV{DESTDIR}\")
        set(_tb_legacy \"\${_tb_root}/etc/gbox\")
        set(_tb_target \"\${_tb_root}${TB_INSTALL_SYSCONF_DIR}/tuxibit/tuxibit.conf\")
        file(MAKE_DIRECTORY \"\${_tb_legacy}\")
        if(EXISTS \"\${_tb_target}\" AND NOT EXISTS \"\${_tb_legacy}/tuxibit.conf\")
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E create_symlink
                    \"../tuxibit/tuxibit.conf\"
                    \"\${_tb_legacy}/tuxibit.conf\"
            )
        endif()
    endif()
")

# Deploy assets (I210 production tools)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deploy/bin/eepromARMtool")
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/deploy/bin/eepromARMtool
        DESTINATION ${CMAKE_INSTALL_DATADIR}/tuxibit/deploy/bin
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deploy/data/I210NIC-origin.otp")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/deploy/data/I210NIC-origin.otp
        DESTINATION ${CMAKE_INSTALL_DATADIR}/tuxibit/deploy/data
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resource/fonts/LiHeiPro.ttf")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/fonts/LiHeiPro.ttf
        DESTINATION ${CMAKE_INSTALL_DATADIR}/tuxibit/fonts
    )
endif()

# Optional: version info template
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config/tuxibit-version.conf.example")
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/tuxibit-version.conf.example
        DESTINATION ${CMAKE_INSTALL_DATADIR}/tuxibit
    )
endif()

# Post-install: refresh icon cache & desktop database when installing on target system
install(CODE "
    if(NOT \"\$ENV{DESTDIR}\")
        find_program(_TB_GTK_ICON_CACHE gtk-update-icon-cache)
        if(_TB_GTK_ICON_CACHE)
            execute_process(
                COMMAND \${_TB_GTK_ICON_CACHE} -f -t
                    \"${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor\"
                RESULT_VARIABLE _tb_icon_cache_result
                ERROR_QUIET
            )
        endif()
        find_program(_TB_UPDATE_DESKTOP_DB update-desktop-database)
        if(_TB_UPDATE_DESKTOP_DB)
            execute_process(
                COMMAND \${_TB_UPDATE_DESKTOP_DB}
                    \"${CMAKE_INSTALL_FULL_DATADIR}/applications\"
                RESULT_VARIABLE _tb_desktop_db_result
                ERROR_QUIET
            )
        endif()
    endif()
")

# sergeykhbr@gmail.com
#
cmake_minimum_required(VERSION 3.16)

project(qtmonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{QT_PATH})
#set(INSTALL_QTMONITORDIR "bin")

find_package(Qt6 REQUIRED COMPONENTS Core Gui SerialPort Widgets)
qt_standard_project_setup()

qt_add_executable(qtmonitor
    src/comsettings.cpp
    src/comsettings.h
    src/console.cpp
    src/console.h
    src/ctrlpanel.cpp
    src/ctrlpanel.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/main.cpp
)

set(qtmonitor_resources
    "images/logo.png"
    "images/splashscreen.png"
    "images/1_37x32x4.png"
    "images/2_37x32x4.png"
    "images/appexit.png"
    "images/connect.png"
    "images/disconnect.png"
    "images/clear.png"
    "images/settings.png"
)

qt_add_resources(qtmonitor imageresources
    PREFIX "/"
    FILES ${qtmonitor_resources}
)

target_link_libraries(qtmonitor PRIVATE
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
)

set_target_properties(qtmonitor PROPERTIES
    WIN32_EXECUTABLE ON
)

#install(TARGETS qtmonitor
#    RUNTIME DESTINATION "${INSTALL_QTMONITORDIR}"
#    BUNDLE DESTINATION "${INSTALL_QTMONITORDIR}"
#    LIBRARY DESTINATION "${INSTALL_QTMONITORDIR}"
#)
#qt_generate_deploy_app_script(
#    TARGET qtmonitor
#    OUTPUT_SCRIPT deploy_script
#)
#install(SCRIPT ${deploy_script})
set_target_properties(qtmonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_BINDIR}")
set_target_properties(qtmonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_BINDIR}")
set_target_properties(qtmonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_BINDIR}")


if(UNIX)
    add_custom_command(TARGET qtmonitor
	        POST_BUILD
	        COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/post_build_event.sh "${CMAKE_BINARY_DIR}"
	        )

else()
    add_custom_command(TARGET qtmonitor
	        POST_BUILD
	        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/qtdeploy.bat "${WINDEPLOYQT_EXECUTABLE}" "\"$(OutDir)$(TargetName)$(TargetExt)\""
	        )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT qtmonitor)
endif()


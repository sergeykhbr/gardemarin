# sergeykhbr@gmail.com
#
cmake_minimum_required(VERSION 3.16)

project(qtmonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.7.0/msvc2019_64")
set(INSTALL_QTMONITORDIR "bin/qtmonitor")

find_package(Qt6 REQUIRED COMPONENTS Core Gui SerialPort Widgets)
qt_standard_project_setup()

qt_add_executable(qtmonitor
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

install(TARGETS qtmonitor
    RUNTIME DESTINATION "${INSTALL_QTMONITORDIR}"
    BUNDLE DESTINATION "${INSTALL_QTMONITORDIR}"
    LIBRARY DESTINATION "${INSTALL_QTMONITORDIR}"
)

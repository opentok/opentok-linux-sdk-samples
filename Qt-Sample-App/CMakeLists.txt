cmake_minimum_required(VERSION 3.12)

project(Qt-Sample-App VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS OpenGL OpenGLWidgets Widgets Multimedia MultimediaWidgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        glvideowidget.cpp
        glvideowidget.h
        participant.h
        qtsession.h
        qtsession.cpp
        qtpublisher.h
        qtpublisher.cpp
        qtsubscriber.h
        qtsubscriber.cpp
)

add_executable(qt_sample_app
        ${PROJECT_SOURCES}
)

target_include_directories(qt_sample_app PRIVATE /home/parallels/opentok/include)
target_link_directories(qt_sample_app PRIVATE /home/parallels/opentok/lib)
find_library(OTK opentok PATHS /home/parallels/opentok/lib)
target_link_libraries(qt_sample_app Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::MultimediaWidgets ${OTK})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qt-Sample-App)
endif()

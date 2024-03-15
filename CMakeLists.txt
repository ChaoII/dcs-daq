cmake_minimum_required(VERSION 3.20)
project(dcsdaq)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(FD_DIRECTORY "E:/Fastdeploy/build/install" CACHE PATH "fastdeploy install directory")
include(${FD_DIRECTORY}/FastDeploy.cmake)

find_package(Qt6 COMPONENTS Widgets Multimedia OpcUa REQUIRED)

set(PROJECT_SOURCE_DIR
        src/agraphicsview.cpp
        src/agraphicsview.h
        src/agraphicsview.ui
        src/crossitem.h
        src/crossitem.cpp
        src/arectlist.h
        src/arectlist.cpp
        src/mainwindow.h
        src/mainwindow.cpp
        src/mainwindow.ui
        src/tempgraphicsitem.h
        src/tempgraphicsitem.cpp
        src/arectitem.h
        src/arectitem.cpp
        src/acamerapro.h
        src/acamerapro.cpp
        src/arectlistitem.h
        src/arectlistitem.cpp
        src/arectlistitem.ui
        src/ataginputdialog.h
        src/ataginputdialog.cpp
        src/ataginputdialog.ui
        src/ocr/ppocrv4.h
        src/ocr/ppocrv4.cpp
        src/utils/utils.h
        src/utils/utils.cpp
        src/utils/config.h
        src/utils/config.cpp
        src/opc/opc.h
        src/opc/opc.cpp
        qrc.qrc
        main.cpp
)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${FASTDEPLOY_INCS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::OpcUa
        ${FASTDEPLOY_LIBS})










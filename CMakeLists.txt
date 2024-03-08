cmake_minimum_required(VERSION 3.20)
project(dcsdaq)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

set(PROJECT_SOURCE_DIR
        src/agraphicsview.cpp
        src/agraphicsview.h
        src/agraphicsview.ui
        src/crossitem.h
        src/crossitem.cpp
        src/itemlist.h
        src/itemlist.cpp
        src/itemlist.ui
        src/mainwindow.h
        src/mainwindow.cpp
        src/mainwindow.ui
        src/tempgraphicsitem.h
        src/tempgraphicsitem.cpp
        src/arectitem.h
        src/arectitem.cpp
        src/acamerapro.h
        src/acamerapro.cpp
        qrc.qrc
        main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)










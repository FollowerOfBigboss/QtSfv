cmake_minimum_required(VERSION 3.16.0)

project ("QtSfv")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)
message(STATUS "Using Qt${QT_VERSION_MAJOR}")
set(QT_QMAKE_EXECUTABLE "${CMAKE_PREFIX_PATH}/bin/qmake.exe")

add_executable(ExeQtSfv
	QtSfv/main.cpp
	QtSfv/appwindow.h
	QtSfv/appwindow.cpp
	QtSfv/crc32/CRC.h
)

target_link_libraries(ExeQtSfv Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core)

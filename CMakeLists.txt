cmake_minimum_required(VERSION 3.5)

project(IoTParkingSystem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core SerialPort REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Tesseract 4)
include_directories(${OpenCV_INCLUDE_DIRS})

if (NOT Tesseract_FOUND)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(Tesseract REQUIRED IMPORTED_TARGET tesseract lept)
    add_library(Tesseract::libtesseract ALIAS PkgConfig::Tesseract)
endif ()

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        arduino.cpp
        car.cpp
        widget.h
        arduino.h
        car.h
        widget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(IoTParkingSystem
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(IoTParkingSystem SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(IoTParkingSystem
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(IoTParkingSystem PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(IoTParkingSystem PRIVATE Qt5::Core Qt5::SerialPort)
target_link_libraries(IoTParkingSystem PRIVATE ${OpenCV_LIBS})
target_link_libraries(IoTParkingSystem PRIVATE Tesseract::libtesseract)


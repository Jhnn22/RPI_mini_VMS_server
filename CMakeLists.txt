cmake_minimum_required(VERSION 3.16)

project(RPI_mini_VMS_server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# GStreamer package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED
    gstreamer-1.0
    gstreamer-video-1.0
)

# GStreamer 라이브러리 및 포함 경로 설정
include_directories(${GSTREAMER_INCLUDE_DIRS})
link_directories(${GSTREAMER_LIBRARY_DIRS})

# ui 폴더에 있는 ui 파일을 읽을 수 있도록 설정
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(PROJECT_HEADERS
        include/mainwidget.h
        include/gstthread.h
        include/displaywidget.h
        include/httplib.h
        include/device.h
        include/devicemanager.h
        include/deviceregisterdialog.h
)

set(PROJECT_SOURCES
        source/mainwidget.cpp
        source/gstthread.cpp
        source/displaywidget.cpp
        source/device.cpp
        source/devicemanager.cpp
        source/deviceregisterdialog.cpp
)

set(PROJECT_UI
        ui/mainwidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RPI_mini_VMS_server
        MANUAL_FINALIZATION
        main.cpp
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        ${PROJECT_UI}
        data.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RPI_mini_VMS_server APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RPI_mini_VMS_server SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_UI}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RPI_mini_VMS_server
            ${PROJECT_SOURCES}
            ${PROJECT_UI}
        )
    endif()
endif()

target_link_libraries(RPI_mini_VMS_server PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${GSTREAMER_LIBRARIES}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RPI_mini_VMS_server)
endif()
set_target_properties(RPI_mini_VMS_server PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RPI_mini_VMS_server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RPI_mini_VMS_server)
endif()

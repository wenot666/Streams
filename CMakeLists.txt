cmake_minimum_required(VERSION 3.16)

project(Test_two VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network SerialPort PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network SerialPort PrintSupport)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    serialportreader.cpp
    serialportreader.h
    ethernetreader.cpp
    ethernetreader.h
    qcustomplot.cpp
    qcustomplot.h
    dataemitter.cpp
    dataemitter.h

)

add_executable(Test_two ${PROJECT_SOURCES})

target_link_libraries(Test_two PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(Test_two PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Test_two
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Test_two)
endif()

cmake_minimum_required(VERSION 3.14)

project(Windows_Service LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Concurrent Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Concurrent Network)

add_executable(Windows_Service
  main.cpp
  worker.h
  worker.cpp
  win_api.h
  win_api.cpp
)
target_link_libraries(Windows_Service PRIVATE Wtsapi32 ws2_32 iphlpapi Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Concurrent  Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS Windows_Service
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(ICON_PATH "C:/Users/syedr/OneDrive/Documents/New folder/FYP_RENEW/Windows_Service/logo/icons8-services-32.ico")
target_sources(Windows_Service PRIVATE ${ICON_PATH})

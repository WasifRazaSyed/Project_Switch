cmake_minimum_required(VERSION 3.5)

project(Windows_Service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Windows_Service main.cpp)

include(GNUInstallDirs)
install(TARGETS Windows_Service
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

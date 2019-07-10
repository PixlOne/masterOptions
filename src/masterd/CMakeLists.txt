cmake_minimum_required(VERSION 3.14)
project(masterd)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../CMake")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

add_executable(masterd
    masterd.cpp
    Configuration.cpp
    Configuration.h
    DeviceFinder.cpp
    DeviceFinder.h
    DeviceHandler.cpp
    DeviceHandler.h
    Diverter.cpp
    Diverter.h
    EvdevDevice.cpp
    EvdevDevice.h
    EventHandler.cpp
    EventHandler.h
    Listener.cpp
    Listener.h
    Logger.cpp
    Logger.h)

set_target_properties(masterd PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

pkg_check_modules(PC_EVDEV libevdev)
pkg_check_modules(libhidpp REQUIRED)
pkg_check_modules(libconfig++ REQUIRED)

find_path(HIDPP_INCLUDE_DIR hidpp)
find_library(HIDPP_LIBRARY libhidpp.so)

find_path(EVDEV_INCLUDE_DIR libevdev/libevdev.h
          HINTS ${PC_EVDEV_INCLUDE_DIRS} ${PC_EVDEV_INCLUDEDIR})
find_library(EVDEV_LIBRARY
        NAMES evdev libevdev)

include_directories(${HIDPP_INCLUDE_DIR}/hidpp ${EVDEV_INCLUDE_DIR})

target_link_libraries(masterd ${CMAKE_THREAD_LIBS_INIT} ${EVDEV_LIBRARY} config++ ${HIDPP_LIBRARY})

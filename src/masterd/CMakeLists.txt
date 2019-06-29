cmake_minimum_required(VERSION 3.14)
project(masterd)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

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

include_directories("/usr/local/include/hidpp/" "/usr/include/libevdev-1.0/")

if(EXISTS "/usr/local/lib/libhidpp.so")
    target_link_libraries(masterd /usr/local/lib/libhidpp.so)
else()
    target_link_libraries(masterd /usr/lib/libhidpp.so)
endif()

target_link_libraries(masterd ${CMAKE_THREAD_LIBS_INIT} /usr/lib/libevdev.so /usr/lib/libconfig++.so)
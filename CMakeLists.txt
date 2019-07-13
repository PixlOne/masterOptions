cmake_minimum_required(VERSION 3.10)
project(masterOptions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/masterd)

if(NOT EXISTS "${PROJECT_BINARY_DIR}/masterd.cfg")
    configure_file("masterd.example.cfg" "masterd.cfg" COPYONLY)
endif()

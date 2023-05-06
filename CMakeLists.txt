cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -fsycl-targets=nvptx64-nvidia-cuda -O3")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES src/main.cpp)
set(TARGET main)

add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PUBLIC "${CMAKE_SOURCE_DIR}/vendor/sciplot")



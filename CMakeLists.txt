cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -fsycl-targets=nvptx64-nvidia-cuda")

set(SOURCES src/main.cpp)

add_executable(main ${SOURCES})



cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -fsycl-targets=nvptx64-nvidia-cuda -O3 -lglfw")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

FILE(GLOB SOURCES	${CMAKE_SOURCE_DIR}/src/* )
set(SOURCES ${SOURCES} ${CMAKE_SOURCE_DIR}/src/glad/glad.c)
set(TARGET main)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${GLFW_LIBRARIES})
target_include_directories(${TARGET} PUBLIC "${CMAKE_SOURCE_DIR}/vendor/sciplot")



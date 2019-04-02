if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-tree build banned. Aborting")
endif()

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(AkarinNative)

add_subdirectory(SDL2)

file(GLOB_RECURSE SRC "Source/*.*")
add_library(AkarinNative SHARED ${SRC})
target_include_directories(AkarinNative PRIVATE "Source")
target_link_libraries(AkarinNative PRIVATE SDL2-static)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-tree build banned. Aborting")
endif()

cmake_minimum_required(VERSION 3.10)

include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(AKARIN_BIN_ROOT "${CMAKE_SOURCE_DIR}/../Bin/$<CONFIG>/netcoreapp2.1/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AKARIN_BIN_ROOT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AKARIN_BIN_ROOT})

project(AkarinNative)

add_subdirectory(SDL2)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC "Source/*.*")
add_library(AkarinNative SHARED ${SRC})
target_include_directories(AkarinNative PRIVATE "Source")
target_link_libraries(AkarinNative PRIVATE SDL2-static Vulkan::Vulkan)

check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    set_property(TARGET AkarinNative PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Debug>:FALSE>:TRUE)
else()
    message(WARNING "IPO is not supported on current platform")
endif()

set_target_properties(AkarinNative PROPERTIES PREFIX "")

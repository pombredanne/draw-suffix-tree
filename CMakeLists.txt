cmake_minimum_required(VERSION 2.8.7)

project(draw-suffix-tree CXX C)
set(CMAKE_BUILD_TYPE "Release")

set (CMAKE_CXX_FLAGS "--std=c++11")

add_subdirectory(sdsl-lite)

include_directories( ${CMAKE_BINARY_DIR}/sdsl-lite/include
                     ${CMAKE_BINARY_DIR}/sdsl-lite/external/libdivsufsort/include/
                    )

add_executable(draw-suffix-tree.x draw-suffix-tree.cpp)
target_link_libraries(draw-suffix-tree.x sdsl divsufsort divsufsort64)


cmake_minimum_required(VERSION 3.8)

project(Tetris)

include_directories(${CMAKE_BINARY_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

add_executable(Tetris src/main.cpp)
target_link_libraries(Tetris ${CONAN_LIBS})

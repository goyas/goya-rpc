cmake_minimum_required(VERSION 3.9)
project(goya-rpc)

add_compile_options(-std=c++11)
add_definitions("-ggdb3")

set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#添加头文件路径，相当于makefile里面的-I
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/install/include)

#添加库文件搜索路径，相当于makefile里面的-L
link_directories(${PROJECT_SOURCE_DIR}/thirdparty/install/lib)

include(config.cmake)
add_subdirectory(src)
add_subdirectory(sample/echo)

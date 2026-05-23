cmake_minimum_required(VERSION 3.8)
project(BinaryTreeUI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(BinaryTreeUI WIN32 src/main.cpp src/Tree.cpp)

target_include_directories(BinaryTreeUI PRIVATE include)
if(MSVC)
  target_compile_definitions(BinaryTreeUI PRIVATE UNICODE _UNICODE)
endif()

target_link_libraries(BinaryTreeUI PRIVATE gdiplus comdlg32)

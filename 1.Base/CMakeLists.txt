cmake_minimum_required(VERSION 3.15)
project(openGL-Window)

add_executable(a.out main.cpp)
target_include_directories(a.out PUBLIC ${CMAKE_SOURCE_DIR}/glad)

add_subdirectory(glad)
target_link_libraries(a.out glad)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(a.out glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(a.out OpenGL::GL)
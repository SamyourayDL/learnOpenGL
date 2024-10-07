cmake_minimum_required(VERSION 3.15)
project(opengl)

# Change to compile other lesson
add_subdirectory(5.Light)

target_include_directories(a.out PUBLIC ${CMAKE_SOURCE_DIR}/lib)
# target_include_directories(a.out PUBLIC ${CMAKE_SOURCE_DIR}/stb)

add_subdirectory(glad)
target_include_directories(a.out PUBLIC ${CMAKE_SOURCE_DIR}/glad)
target_link_libraries(a.out glad)

install(TARGETS a.out
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR})





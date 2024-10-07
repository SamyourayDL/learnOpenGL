cmake_minimum_required(VERSION 3.0)

add_library(glad STATIC glad.c glad.h khrplatform.h)

# target_include_directories(
#     glad
#     PRIVATE
#     ${CMAKE_CURRENT_BINARY_DIR}  
# )

# target_include_directories(
#     glad
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
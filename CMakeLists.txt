cmake_minimum_required(VERSION 3.11)
project(litzy)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.*pp)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
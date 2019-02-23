cmake_minimum_required(VERSION 2.8)

project(LearnOpenCV)

# message(STATUS "OpenCV:" ${OpenCV})
find_package(OpenCV REQUIRED)

message("OpenCV_INCLUDE_DIRS:" ${OpenCV_INCLUDE_DIRS})
include_directories (${OpenCV_INCLUDE_DIRS})

aux_source_directory(. SRC_DIR)

add_executable(main ${SRC_DIR})

message(STATUS "OpenCV_LIBS:" ${OpenCV_LIBS})
target_link_libraries(main ${OpenCV_LIBS})

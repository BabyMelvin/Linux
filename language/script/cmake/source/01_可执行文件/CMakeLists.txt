CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(HELLO)

SET(SRC_LIST main.c)
# 多个源文件，可以使用,放入到SRC_LIST变量中
# AUX_SOURCE_DIRECTORY(. SRC_LIST)

# 等价
MESSAGE(STATUS "this is binary dir" ${HELLO_BINARY_DIR})
MESSAGE(STATUS "this is binary dir" ${PROJECT_BINARY_DIR})

# 等价
MESSAGE(STATUS "this is source dir" ${HELLO_SOURCE_DIR})
MESSAGE(STATUS "this is source dir" ${PROJECT_SOURCE_DIR})

# send error,过程跳过
#MESSAGE(SEND_ERROR "this is send error message")

# fatal error,终止编译
#MESSAGE(FATAL_ERROR "this is fata error message")

# 添加math子目录
ADD_SUBDIRECTORY(math)

# 添加头文件目录
INCLUDE_DIRECTORIES(./math)

ADD_EXECUTABLE(HELLO ${SRC_LIST})

# 添加链接表
TARGET_LINK_LIBRARIES(HELLO MathFuncs)

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
# ADD_SUBDIRECTORY(math)

# 添加头文件目录
# INCLUDE_DIRECTORIES(./math)

###########################################################
# 添加版本号
set (Demo_VERSION_MAJOR 1)
set (Demo_VERSION_MINOR 0)
###########################################################

######################config################################
# 加入一个配置文件，用于处理CMAKE对源码设置
configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/config.h"
)

# 是否使用自己的MathFuncs库
option (USE_MYMATH "use provided math implemention" ON)

if (USE_MYMATH)
    include_directories ("${PROJECT_SOURCE_DIR}/math")
    add_subdirectory (math)
    set (EXTRA_LIBS ${EXTRA_LIBS} MathFuncs)
endif (USE_MYMATH)
#######################################################

###########################################################
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "ENV{CXXFLAGS} -O3 -Wall")
###########################################################

#########################################################
# 检查环境
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
check_function_exists (pow HAVE_POW)
#########################################################

LINK_LIBRARIES(m)

ADD_EXECUTABLE(HELLO ${SRC_LIST})

# 添加链接表
TARGET_LINK_LIBRARIES(HELLO ${EXTRA_LIBS})

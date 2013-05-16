cmake_minimum_required(VERSION 2.8)
project(test)


####Specify the path of ctpp
set(CTPP_ROOT_DIR /home/sherry/ChinesePreProcessing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR} ${CTPP_ROOT_DIR})
link_directories(${CTPP_ROOT_DIR}/lib)




set(src testCTPP.cpp
)

add_executable(test ${src})
target_link_libraries(test ctpp)
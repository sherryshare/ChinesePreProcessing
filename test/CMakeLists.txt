cmake_minimum_required(VERSION 2.8)
project(test)


####Specify the path of ctpp
set(CTPP_ROOT_DIR /home/sherry/ChinesePreProcessing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR} ${CTPP_ROOT_DIR})
link_directories(${CTPP_ROOT_DIR}/lib)




set(bin_src testBin.cpp)
set(sen_src testSen.cpp)

add_executable(test_bin ${bin_src})
target_link_libraries(test_bin ctpp)
add_executable(test_sen ${sen_src})
target_link_libraries(test_sen ctpp)
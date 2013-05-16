cmake_minimum_required(VERSION 2.8)
project(ctpp)



set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include_directories(${PROJECT_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR}/lib)


set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


set(src file_processing.cpp
)

add_library(ctpp_dynamic SHARED ${src})
add_library(ctpp_static STATIC ${src})




set_target_properties(ctpp_dynamic PROPERTIES OUTPUT_NAME "ctpp")
set_target_properties(ctpp_static PROPERTIES OUTPUT_NAME "ctpp")
set_target_properties(ctpp_dynamic PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ctpp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

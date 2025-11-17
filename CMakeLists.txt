cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Test/run/)

set(PRJ_NAME test_step)
project(${PRJ_NAME})

include_directories(StepVer1)
include_directories(Tool)
include_directories(./Tool/jsoncpp)

set(SOURCES 
    ./StepVer1/StreamBase.cpp
    ./StepVer1/MultiRecordStream.cpp
    ./Tool/jsoncpp/jsoncpp.cpp
    ./Tool/StringFunc.cc
)

add_definitions(-DFMT_HEADER_ONLY)
#add_definitions(-DJEMALLOC_NO_DEMANGLE)

add_executable(${PRJ_NAME} ./Test/main.cpp ${SOURCES})
#target_link_libraries(${PRJ_NAME} jemalloc)
#target_link_libraries(${PRJ_NAME} tcmalloc)

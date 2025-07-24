cmake_minimum_required(VERSION 3.10)
project(StepVer2Test)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Tool)

# 查找源文件
file(GLOB NEWSTEP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../StepVer2/*.cc"
)

file(GLOB TOOLS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../Tool/StringFunc.cc"
)

file(GLOB ORIGINAL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../CachedPBStep.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ReuseCacheList.cc"
)

# 创建新实现的库
add_library(stepver2 STATIC
    ${NEWSTEP_SOURCES}
    ${TOOLS_SOURCES}
)

# 创建原实现的库（用于对比测试）
add_library(originalpbstep STATIC
    ${ORIGINAL_SOURCES}
    ${TOOLS_SOURCES}
)

# 基本功能测试
add_executable(test_stepver2
    test_stepver2.cpp
)

target_link_libraries(test_stepver2
    stepver2
)

# 简单性能测试
add_executable(test_simple_comparison
    test_simple_comparison.cpp
)

target_link_libraries(test_simple_comparison
    stepver2
)

# 使用示例
add_executable(example_usage
    example_usage.cpp
)

target_link_libraries(example_usage
    stepver2
)

# 添加测试目标
enable_testing()

add_test(NAME BasicFunctionality COMMAND test_stepver2)
add_test(NAME PerformanceTest COMMAND test_simple_comparison)

# 自定义目标：运行所有测试
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_stepver2 test_simple_comparison
    COMMENT "Running all tests"
)

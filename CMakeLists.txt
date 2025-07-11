cmake_minimum_required(VERSION 3.5)

project(mygo VERSION 0.1 LANGUAGES C CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()


add_subdirectory(./third_party/googletest-1.17.0)
include_directories(./third_party/googletest-1.17.0/googletest/include)

set( LIB_SOURCES
        ./vm.cpp
        ./token.cpp
        ./token_stream.cpp
        ./logging.cpp
        ./ast.cpp
        ./frame.cpp
)

add_library(mygodev STATIC ${LIB_SOURCES})


set(CMAKE_BUILD_TYPE Debug)
message(${LIB_SOURCES})

add_executable(mygo mygo.cpp)
target_link_libraries(mygo PRIVATE mygodev)

#add_executable(test_func test/test_func.cpp)
#target_link_libraries(test_func PRIVATE mygodev)
#
#add_executable(test_decl test/test_decl.cpp)
#target_link_libraries(test_decl PRIVATE mygodev)

add_executable(test_ret test/test_ret.cpp)
target_link_libraries(test_ret GTest::gtest_main mygodev)

add_executable(test_expr test/test_expr.cpp)
target_link_libraries(test_expr GTest::gtest_main mygodev)

add_executable(test_if test/test_if.cpp)
target_link_libraries(test_if GTest::gtest_main mygodev)

#add_executable(mygo ${PROJECT_SOURCES})
#add_executable(test_all ${TEST_SOURCES})

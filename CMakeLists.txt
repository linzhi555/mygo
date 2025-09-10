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


# dependencies
add_subdirectory(./third_party/libuv-1.51.0)
include_directories(third_party/libuv-1.51.0/include)

add_subdirectory(./third_party/googletest-1.17.0)
include_directories(./third_party/googletest-1.17.0/googletest/include)

add_subdirectory(./third_party/spdlog-1.15.3)
include_directories(./third_party/spdlog-1.15.3/include)



include_directories(./src)

set( LIB_SOURCES

        ./src/loop/loop.cpp
        ./src/loop/task.cpp
        ./src/compiler.cc
        ./src/vm.cpp
        ./src/token.cpp
        ./src/token_stream.cpp
        ./src/server.cpp
        ./src/ast.cpp
        ./src/value.cpp
)

add_library(mygodev STATIC ${LIB_SOURCES})
target_link_libraries(mygodev PRIVATE uv_a )


set(CMAKE_BUILD_TYPE Debug)
message(${LIB_SOURCES})

add_executable(mygo ./src/mygo.cpp)
target_link_libraries(mygo PRIVATE mygodev)


# add_executable(test_ret test/test_ret.cpp)
# target_link_libraries(test_ret GTest::gtest_main mygodev)

# add_executable(test_expr test/test_expr.cpp)
# target_link_libraries(test_expr GTest::gtest_main mygodev)

# add_executable(test_if test/test_if.cpp)
# target_link_libraries(test_if GTest::gtest_main mygodev)

# add_executable(test_for test/test_for.cpp)
# target_link_libraries(test_for GTest::gtest_main mygodev)

add_executable(test_bytevm test/test_bytevm.cpp)
target_link_libraries(test_bytevm GTest::gtest_main mygodev)

add_executable(test_struct test/test_struct.cpp)
target_link_libraries(test_struct GTest::gtest_main mygodev)

add_executable(test_ast test/test_ast.cpp)
target_link_libraries(test_ast GTest::gtest_main mygodev)

add_executable(test_compiler test/test_compiler.cpp)
target_link_libraries(test_compiler GTest::gtest_main mygodev)



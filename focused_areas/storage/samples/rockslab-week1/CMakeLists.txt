cmake_minimum_required(VERSION 3.16)
project(rockslab_week1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rockslab
  src/main.cc
  src/mini_lsm.cc
)
target_include_directories(rockslab PRIVATE include)
target_compile_options(rockslab PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_executable(mini_lsm_test
  tests/mini_lsm_test.cc
  src/mini_lsm.cc
)
target_include_directories(mini_lsm_test PRIVATE include)
target_compile_options(mini_lsm_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME mini_lsm_test COMMAND mini_lsm_test)

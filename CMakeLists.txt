cmake_minimum_required(VERSION 3.11)
project(data_structures)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(sources src/linked_list.cpp)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(data_structures ${sources})
target_include_directories(data_structures PUBLIC src)

include(CTest)
add_subdirectory(tests)

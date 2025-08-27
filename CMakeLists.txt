cmake_minimum_required(VERSION "3.29")
project("data-structures")

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Catch2 3 REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.hpp" "src/*.cpp")

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if(MSVC)
  target_compile_options(tests PRIVATE /W4 /WX)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

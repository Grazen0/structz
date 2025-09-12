cmake_minimum_required(VERSION 3.11)
project(structz LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(structz
    src/doubly_linked_list.cpp
    src/hash_map.cpp
    src/linked_list.cpp
    src/stack.cpp
    src/vec.cpp)
target_include_directories(structz PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(structz PRIVATE /W4 /WX)
else()
    target_compile_options(structz PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

install(TARGETS structz
    EXPORT structzTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    structzConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT structzTargets
    FILE structzConfig.cmake
    NAMESPACE structz::
    DESTINATION lib/cmake/structz
)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/structzConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/structzConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/structz
)


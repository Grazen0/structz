find_package(Catch2 3 REQUIRED)

set(test_sources test_linked_list.cpp)

foreach(test_source ${test_sources})
    get_filename_component(test_name ${test_source} NAME_WE)
    set(test_exec ${test_name})

    add_executable(${test_exec} ${test_source})
    target_link_libraries(${test_exec} PRIVATE structz)
    target_link_libraries(${test_exec} PRIVATE Catch2::Catch2WithMain)

    add_test(NAME ${test_name} COMMAND ${test_exec})
endforeach()

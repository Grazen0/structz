find_package(Catch2 3 REQUIRED)

set(TEST_SOURCES
    test_avl_tree.cpp
    test_binary_heap.cpp
    test_circular_list.cpp
    test_linked_list.cpp
    test_hash_map.cpp
    test_hash_set.cpp
    test_doubly_linked_list.cpp
    test_queue.cpp
    test_red_black_tree.cpp
    test_stack.cpp
    test_vec.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    set(TEST_EXEC ${TEST_NAME})

    add_executable(${TEST_EXEC} ${TEST_SOURCE})
    target_link_libraries(${TEST_EXEC} PRIVATE structz)
    target_link_libraries(${TEST_EXEC} PRIVATE Catch2::Catch2WithMain)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_EXEC})
endforeach()

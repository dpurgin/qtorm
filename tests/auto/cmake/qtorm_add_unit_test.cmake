function(qtorm_add_unit_test)
    set(OPTIONS)
    set(ONE_VALUE_ARGS NAME)
    set(MULTI_VALUE_ARGS SOURCES LINK_LIBRARIES)

    cmake_parse_arguments(QTORM_ADD_UNIT_TEST "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    add_executable(${QTORM_ADD_UNIT_TEST_NAME} ${QTORM_ADD_UNIT_TEST_SOURCES})
    target_link_libraries(${QTORM_ADD_UNIT_TEST_NAME} Qt5::Test qtorm ${QTORM_ADD_UNIT_TEST_LINK_LIBRARIES})

    add_test(NAME ${QTORM_ADD_UNIT_TEST_NAME} COMMAND ${QTORM_ADD_UNIT_TEST_NAME})

    if (WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.22.0")
        set_tests_properties(${QTORM_ADD_UNIT_TEST_NAME} PROPERTIES
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${CMAKE_BINARY_DIR}/src;PATH=path_list_prepend:${_qt5Core_install_prefix}/bin")
    endif()
endfunction()
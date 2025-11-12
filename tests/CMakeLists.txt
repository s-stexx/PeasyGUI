enable_testing()

function(SETUP_TEST _name)
    set(TEST_SRC
        test_${_name}.cpp
        ../src/${_name}.h
        ../src/${_name}.cpp
    )

    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs EXTRA_FILES)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    list(APPEND TEST_SRC ${ARGN})
    add_executable(${_name} ${TEST_SRC})
    add_test(NAME ${_name} COMMAND ${_name})

    target_link_libraries(${_name}
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
    )
endfunction()

SETUP_TEST(
    requestparser
)

SETUP_TEST(
    requestchecker
    mockcontrol.h
    ../src/request.h
    ../src/control.h ../src/control.cpp
    ../src/controlprototypes.h ../src/controlprototypes.cpp
)


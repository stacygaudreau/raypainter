##########################################
# QTest config
##########################################
file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    # a binary is created for each test .cpp
    qt_add_executable(${TEST_NAME} ${TEST_FILE} ${SRC_SOURCES})
    # include GUI source and link any libraries needed
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${TEST_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
    # IS_TEST_SUITE macro added for test binaries
    target_compile_definitions(${TEST_NAME} PRIVATE IS_TEST_SUITE)
    # register w/ ctest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

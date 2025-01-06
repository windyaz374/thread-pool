# Create library target
add_library(thread_pool STATIC
    src/post.cc
    src/threadPool.cc
)

# Set include directories for the library
target_include_directories(thread_pool
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Export the library
set_target_properties(thread_pool
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add the test executable
add_executable(thread_pool_test
    test/threadPoolTest.cpp
)

# Link test executable with the library
target_link_libraries(thread_pool_test
    PRIVATE
        thread_pool
        GTest::gtest_main
)

# Add include directories for test
target_include_directories(thread_pool_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Add the test to CTest
add_test(
    NAME thread_pool_test
    COMMAND thread_pool_test
)

# Set output directory for test executable
set_target_properties(thread_pool_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
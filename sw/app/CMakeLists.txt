# Create executables
add_executable(thread_pool_app
    src/threadPoolApp.cc
)

# add_executable(thread_pool_boost
#     src/threadPoolBoostAsio.cc
# )

# Link with thread pool library
target_link_libraries(thread_pool_app
    PRIVATE
        thread_pool
)

# target_link_libraries(thread_pool_boost
#     PRIVATE
#         thread_pool
# )

# Set include directories for both executables
target_include_directories(thread_pool_app
    PRIVATE
        ${THREAD_POOL_INCLUDE_DIRS}
)

# target_include_directories(thread_pool_boost
#     PRIVATE
#         ${THREAD_POOL_INCLUDE_DIRS}
# )

# Set output directory for executables
set_target_properties(thread_pool_app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
# Add subdirectories for library and application
add_subdirectory(lib)
add_subdirectory(app)

# Make include directories available to all subprojects
set(THREAD_POOL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/inc
    CACHE INTERNAL "Thread Pool include directories"
)
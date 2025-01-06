include(FetchContent)

# Set common variables for Google dependencies
set(GOOGLE_DEPS_BUILD_DIR ${CMAKE_BINARY_DIR}/_deps)
set(BUILD_SHARED_LIBS OFF)  # Build static libraries by default

# Google Test
function(setup_google_test)
    # Prevent overriding parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2  # Specify the version you want
        SOURCE_DIR ${GOOGLE_DEPS_BUILD_DIR}/googletest-src
        BINARY_DIR ${GOOGLE_DEPS_BUILD_DIR}/googletest-build
    )
    
    # Check if population has already been performed
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()
endfunction()
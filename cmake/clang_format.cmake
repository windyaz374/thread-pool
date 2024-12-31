# Find clang-format
find_program(CLANG_FORMAT "clang-format")

# Function to get all source files
function(get_source_file _source_file_var)
    file(GLOB_RECURSE _source_files
        ${CMAKE_SOURCE_DIR}/sw/app/src/*.cc
        ${CMAKE_SOURCE_DIR}/sw/app/src/*.h
        ${CMAKE_SOURCE_DIR}/sw/lib/src/*.cc
        ${CMAKE_SOURCE_DIR}/sw/lib/inc/*.h
        ${CMAKE_SOURCE_DIR}/sw/lib/test/*.cc
    )
    set(${_source_file_var} ${_source_files} PARENT_SCOPE)
endfunction()

# Add clang-format target if found
if(CLANG_FORMAT)
    # Get all source files
    get_source_file(ALL_SOURCE_FILES)

    # Add target to check formatting
    add_custom_target( format-check
        COMMAND ${CLANG_FORMAT} 
        --style=file 
        --dry-run
        --Werror
        ${ALL_SOURCE_FILES}
        COMMENT "Checking code formatting with clang-format"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    # Add target to apply formatting
    add_custom_target( format
        COMMAND ${CLANG_FORMAT} 
        --style=file 
        -i
        ${ALL_SOURCE_FILES}
        COMMENT "Applying code formatting with clang-format"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    message(WARNING "clang-format not found. Format target will not be available.")
endif()
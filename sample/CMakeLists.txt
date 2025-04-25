
function(add_sample sample_name)
    file(GLOB_RECURSE source_files ${sample_name}/*.cpp)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_BINARY_DIR}/${sample_name})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${CMAKE_CURRENT_BINARY_DIR}/${sample_name})

    add_executable(${sample_name})
    target_sources(${sample_name} PRIVATE ${source_files})
    target_link_libraries(${sample_name} PUBLIC ${PROJECT_NAME})
endfunction()

add_sample(triangle)
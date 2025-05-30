add_library(rendering_common)
target_sources(rendering_common PRIVATE common/common.cpp)
target_link_libraries(rendering_common PUBLIC ${PROJECT_NAME})

function(add_rendering_sample sample_name)
    add_sample(${sample_name})
    target_include_directories(${sample_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)
    target_link_libraries(${sample_name} PUBLIC rendering_common)
    add_custom_command(
        TARGET
        ${sample_name}
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${THIRD_PARTY_DLLS} $<TARGET_FILE_DIR:${sample_name}>
    )
endfunction()

add_rendering_sample(triangle)
add_rendering_sample(quad)
add_rendering_sample(box)
add_rendering_sample(post-processing)
add_rendering_sample(ray-tracing)
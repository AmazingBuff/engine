add_library(geometry_common)
target_sources(geometry_common PRIVATE common/common.cpp)
target_link_libraries(geometry_common PUBLIC ${PROJECT_NAME})

function(add_geometry_sample sample_name)
    add_sample(${sample_name})
    target_include_directories(${sample_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)
    target_link_libraries(${sample_name} PUBLIC geometry_common)
    add_custom_command(
        TARGET
        ${sample_name}
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${THIRD_PARTY_DLLS} $<TARGET_FILE_DIR:${sample_name}>
    )
endfunction()

add_geometry_sample(convex_hull)
add_geometry_sample(intersect)
add_geometry_sample(construct)
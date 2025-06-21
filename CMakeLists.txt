cmake_minimum_required(VERSION 3.25)

project(
    engine
    LANGUAGES CXX C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options(-Wno-nontrivial-memaccess -Wno-unsafe-buffer-usage)
endif ()

add_subdirectory(ext)
add_subdirectory(3rd)

find_package(Vulkan)

file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(
    ${PROJECT_NAME}
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_precompile_headers(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_LIBS}
    d3d12
    dxgi
    amazing_std
    reflect
    SDL3::SDL3
    Eigen3::Eigen
    assimp::assimp
    spirv-cross-core
)

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
    cxx_std_23
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/"
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/.vscode/compile_commands.json
    VERBATIM
)

if (WIN32)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif ()

add_subdirectory(sample)

EXECUTE_REFLECT()
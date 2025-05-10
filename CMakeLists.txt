cmake_minimum_required(VERSION 3.25)

project(
    engine
    LANGUAGES CXX C
)

add_subdirectory(ext/eigen)
add_subdirectory(ext/assimp)
add_subdirectory(ext/sdl)
add_subdirectory(ext/amazing_std)
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
    include/
    PRIVATE
    src/
)

target_precompile_headers(
    ${PROJECT_NAME}
    PRIVATE
    src/pch.h
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_LIBS}
    d3d12
    dxgi
    amazing_std
    SDL3::SDL3
    Eigen3::Eigen
    assimp::assimp
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

if (WIN32)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        NOMINMAX
    )
endif ()

if (MSVC)
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        # make __VA_OPT__ valid in msvc
        /Zc:preprocessor
        # disable alternative tokens
        /Za
        # utf-8 source and execution character sets
        /utf-8
    )
endif ()

add_subdirectory(sample)
function(add_lib lib_name libs dlls include_dirs)

set(current_libs "${${libs}}")
set(current_dlls "${${dlls}}")
set(current_include_dirs "${${include_dirs}}")


if (WIN32)
    file(GLOB LIBS ${lib_name}/lib/x64/*.lib)
    file(GLOB DLLS ${lib_name}/bin/x64/*.dll)
endif()

list(APPEND current_libs ${LIBS})
list(APPEND current_dlls ${DLLS})
list(APPEND current_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/include/)

set(${libs} ${current_libs} PARENT_SCOPE)
set(${dlls} ${current_dlls} PARENT_SCOPE)
set(${include_dirs} ${current_include_dirs} PARENT_SCOPE)

endfunction()

add_lib(dxc LIBS DLLS INCLUDE_DIRS)
add_lib(vulkan LIBS DLLS INCLUDE_DIRS)
add_lib(renderdoc LIBS DLLS INCLUDE_DIRS)

set(THIRD_PARTY_LIBS ${LIBS} CACHE STRING "" FORCE)
set(THIRD_PARTY_DLLS ${DLLS} CACHE STRING "" FORCE)
set(THIRD_PARTY_INCLUDE_DIRS ${INCLUDE_DIRS} CACHE STRING "" FORCE)
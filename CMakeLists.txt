cmake_minimum_required (VERSION 3.23)

project (oop2_ex01)
set (MY_AUTHORS "Ahmad_Hirbawy & Masa_AbuDhaim")

include (cmake/CompilerSettings.cmake)

add_executable (${CMAKE_PROJECT_NAME})

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (resources)
include (cmake/Zip.cmake)

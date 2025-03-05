cmake_minimum_required(VERSION 3.10)

project(RegalPlayground)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(interpreter ${SOURCES} "playground/interpret_code.cpp")
    target_compile_definitions(interpreter PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    set_target_properties(interpreter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/release/bin>")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(interpreter_debug ${SOURCES} "playground/interpret_code.cpp")
    target_compile_definitions(interpreter_debug PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)
    set_target_properties(interpreter_debug PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/debug/bin>")
else()
    message(SEND_ERROR "ERROR: -DCMAKE_BUILD_TYPE values are \'Release\' or \'Debug\'.")
endif()

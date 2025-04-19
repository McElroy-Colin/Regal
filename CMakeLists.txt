cmake_minimum_required(VERSION 3.10)

project(RegalPlayground)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(interpreter ${SOURCES} "playground/interpreter.cpp")

target_compile_definitions(interpreter PRIVATE $<$<CONFIG:Release>:NDEBUG>)
set_target_properties(interpreter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")

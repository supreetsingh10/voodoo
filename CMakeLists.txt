cmake_minimum_required(VERSION 3.16..3.30)
project(voodoo DESCRIPTION "My programming language!")

set(PROJECT_DIR "/home/supreetsingh/Documents/SavedPrograms/CPP/Cpp_Projects/turtle/")
set(SOURCE_DIR "${PROJECT_DIR}/src")
set(INCLUDE_DIR "${PROJECT_DIR}/include")

add_executable(voodoo "${SOURCE_DIR}/main.cpp")
include_directories("${INCLUDE_DIR}")
add_subdirectory("${SOURCE_DIR}")
link_directories("${INCLUDE_DIR}")


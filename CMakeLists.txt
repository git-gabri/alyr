cmake_minimum_required(VERSION 3.0.0)
project(alyr)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -s -O2")

file (GLOB_RECURSE alyr_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE alyr_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

set (alyr_INCLUDE_DIRS "")
foreach (_headerFile ${alyr_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND alyr_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES alyr_INCLUDE_DIRS)

add_executable(alyr main.cpp ${alyr_SOURCES})
target_include_directories(alyr PRIVATE ${alyr_INCLUDE_DIRS})

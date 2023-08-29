cmake_minimum_required (VERSION 3.22)
project(jack_test C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(external)

# Set a path to our includes.
target_include_directories(jack_test PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# Link to the Jack libraries.
target_link_libraries(jack_test jack)
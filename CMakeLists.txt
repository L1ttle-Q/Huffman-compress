# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.16)

# Set the project name
project (huffman)

# Create a sources variable with a link to all cpp files to compile
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Add an executable with the above sources
add_executable(huffman ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(huffman
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(huffman PRIVATE cxx_std_11)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
    foreach(case RANGE 1 6)
        add_test(NAME huffman_fixture_${case}
            COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tests/run_tests.py
                --executable $<TARGET_FILE:huffman> --case ${case})
    endforeach()
    foreach(case empty single binary)
        add_test(NAME huffman_${case}
            COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tests/run_tests.py
                --executable $<TARGET_FILE:huffman> --case ${case})
    endforeach()
    set_tests_properties(huffman_fixture_1 huffman_fixture_2 huffman_fixture_3
        huffman_fixture_4 huffman_fixture_5 huffman_fixture_6
        huffman_empty huffman_single huffman_binary PROPERTIES TIMEOUT 70)
endif()

# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

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

target_compile_options(huffman PRIVATE
    -std=c++11
)

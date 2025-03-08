# This is the main CMake configuration file
# Make sure CMake is synced whenever you make the changes
cmake_minimum_required(VERSION 3.24)
project(my-mini-compiler C)

set(CMAKE_C_STANDARD 11)

# Add include directory (this will be needed to add your tokens to your lexer)
include_directories(${PROJECT_SOURCE_DIR}/phase2-w25/include)

# Add executables when needed: Make sure you specify the path to your .c or .h file
add_executable(my-mini-compiler-phase1
        phase1-w25/src/lexer/lexer.c
        phase1-w25/src/dynamic_array.c
        phase1-w25/src/operators.c)

add_compile_options(-Wall -Wextra -Wpedantic)
add_executable(my-mini-compiler-phase2
        phase2-w25/src/lexer/lexer.c
        phase2-w25/src/dynamic_array.c
        phase2-w25/src/operators.c
        phase2-w25/src/parser/grammar.c
        phase2-w25/src/parser/parser.c
        phase2-w25/src/main.c)

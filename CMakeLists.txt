cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mycc VERSION 1.0
                  DESCRIPTION "C Compiler"
                  LANGUAGES C)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES
    src/main.c
    src/lexer/lexer.c
    src/lexer/token.c
    src/parser/parser.c
    src/parser/ast.c
    src/codegen/codegen.c
    src/codegen/asm.c
)

add_executable(mycc ${SOURCES})

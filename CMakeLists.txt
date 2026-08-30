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
    src/util.c
    src/frontend/compile_error/compile_error.c
    src/frontend/lexer/lexer.c
    src/frontend/lexer/token.c
    src/frontend/parser/parser.c
    src/frontend/parser/ast.c
    src/IR/IR.c
    src/backend/codegen/codegen.c
    src/backend/codegen/asm.c
    src/backend/codegen/replace_pseudo_regs.c
    src/backend/codegen/fixup_instructions.c
    src/backend/code_emission/emission.c
)

add_executable(mycc ${SOURCES})

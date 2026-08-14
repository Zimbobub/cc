#include "asm.h"


AsmProgram transform_ast(CProgram ast) {
    AsmProgram program = {
        {
            "main",
            NULL,
            0
        }
    };
    return program;
}
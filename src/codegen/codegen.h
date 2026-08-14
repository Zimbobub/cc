#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdlib.h>
#include <stddef.h>

#include "asm.h"
#include "../parser/ast.h"

AsmProgram transform_program(CProgram ast);

#endif
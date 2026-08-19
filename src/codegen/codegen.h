#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "asm.h"
#include "../IR/IR.h"

AsmProgram transform_program(IRProgram ir);

#endif
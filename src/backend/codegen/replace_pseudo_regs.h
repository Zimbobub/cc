#ifndef REPLACE_PSEUDO_REGS_H
#define REPLACE_PSEUDO_REGS_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "./asm.h"

#define VARIABLE_SIZE 4

void replace_pseudo_registers(AsmProgram* this);

#endif
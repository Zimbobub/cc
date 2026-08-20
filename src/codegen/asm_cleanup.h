#ifndef ASM_CLEANUP_H
#define ASM_CLEANUP_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "./asm.h"

#define VARIABLE_SIZE 4

void replace_pseudo_registers(AsmProgram* this);

#endif
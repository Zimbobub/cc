#ifndef EMISSION_H
#define EMISSION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../codegen/asm.h"

char* emit_asm_operand(AsmOperand* op);
char* emit_asm_instruction(AsmInstruction* instr);
char* emit_asm_function_definition(AsmFunctionDefinition* func);
char* emit_asm_program(AsmProgram* program);



#endif
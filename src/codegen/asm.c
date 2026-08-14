#include "asm.h"

void print_asm_operand(AsmOperand* op) {
    if (op->type == OPERAND_IMMEDIATE) {
        printf("%d", op->operand.immediate);
    } else if (op->type == OPERAND_REGISTER) {
        printf("reg");
    } else {
        printf("unknown");
    }
}

void print_asm_instruction(AsmInstruction* instr, int depth) {
    printf("%*cInstruction {\n", depth, ' ');
    if (instr->type == INSTRUCTION_MOV) {
        printf("%*cmov (", depth+2, ' ');
        print_asm_operand(&instr->instruction.mov.src);
        printf(" to ");
        print_asm_operand(&instr->instruction.mov.dst);
        printf(")\n");

    } else if (instr->type == INSTRUCTION_RET) {
        printf("%*cret\n", depth+2, ' ');
    } else {
        printf("%*cUnknown instruction\n", depth+2, ' ');
    }
    printf("%*c}\n", depth, ' ');
}

void print_asm_function_definition(AsmFunctionDefinition* func, int depth) {
    printf("%*cFunctionDefinition (%s) {\n", depth, ' ', func->name);
    for (size_t i = 0; i < func->n_instructions; ++i) {
        print_asm_instruction(&func->instructions[i], depth+2);
    }
    printf("%*c}\n", depth, ' ');
}
void print_asm_program(AsmProgram* program) {
    printf("AsmProgram {\n");
    print_asm_function_definition(&program->function_definition, 2);
    printf("}\n");
}

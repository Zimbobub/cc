#include "asm.h"

void print_asm_operand(AsmOperand* op) {
    if (op->type == OPERAND_IMMEDIATE) {
        printf("%d", op->inner.immediate);
    } else if (op->type == OPERAND_REGISTER) {
        if (op->inner.reg == REGISTER_RAX) printf("RAX");
        else if (op->inner.reg == REGISTER_RDX) printf("RDX");
        else if (op->inner.reg == REGISTER_R10) printf("R10");
        else if (op->inner.reg == REGISTER_R11) printf("R11");
        else printf("?REG?");
    } else if (op->type == OPERAND_PSEUDO) {
        printf("var(%s)", op->inner.pseudo);
    } else if (op->type == OPERAND_STACK_OFFSET) {
        printf("RSP-%ld", op->inner.stack_offset);
    } else {
        printf("unknown");
    }
}

void print_asm_instruction(AsmInstruction* instr, int depth) {
    // printf("%*cInstruction {\n", depth, ' ');
    if (instr->type == INSTRUCTION_MOV) {
        printf("%*cmov (", depth, ' ');
        print_asm_operand(&instr->inner.mov.src);
        printf(" to ");
        print_asm_operand(&instr->inner.mov.dst);
        printf(")\n");

    } else if (instr->type == INSTRUCTION_UNARY) {
        printf("%*c%c", depth, ' ', instr->inner.unary.op);
        print_asm_operand(&instr->inner.unary.operand);
        printf("\n");
    } else if (instr->type == INSTRUCTION_BINARY) {
        printf("%*c", depth, ' ');
        print_asm_operand(&instr->inner.binary.dst);
        printf("%c=", instr->inner.binary.op);
        print_asm_operand(&instr->inner.binary.src);
        printf("\n");
    } else if (instr->type == INSTRUCTION_IDIV) {
        printf("%*cidiv ", depth, ' ');
        print_asm_operand(&instr->inner.idiv);
        printf("\n");
    } else if (instr->type == INSTRUCTION_CDQ) {
        printf("%*ccdq\n", depth, ' ');
    } else if (instr->type == INSTRUCTION_RET) {
        printf("%*cret\n", depth, ' ');
    } else {
        printf("%*cUnknown instruction\n", depth, ' ');
    }
}

void print_asm_function_definition(AsmFunctionDefinition* func, int depth) {
    printf("%*cFunctionDefinition (%s) stack: %ld {\n", depth, ' ', func->name, func->stack_size);
    for (size_t i = 0; i < func->instructions.size; ++i) {
        print_asm_instruction(&func->instructions.inner[i], depth+2);
    }
    printf("%*c}\n", depth, ' ');
}
void print_asm_program(AsmProgram* program) {
    printf("AsmProgram {\n");
    print_asm_function_definition(&program->function_definition, 2);
    printf("}\n");
}

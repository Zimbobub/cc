#include "emission.h"

void throw_code_emission_err(const char* msg) {
    printf("Code emission error\n");
    printf("%s\n", msg);
    exit(1);
}


// get num digits to represent an int
// i64 max is 19 digits, +1 for negatives
size_t int_digits(int val) {
    char buf[64];
    return snprintf(buf, 31, "%d", val);
}


char* emit_asm_operand(AsmOperand* op) {
    if (op->type == OPERAND_IMMEDIATE) {
        size_t digits = int_digits(op->inner.immediate);

        char* str = malloc(digits + 2); // +1 for the '$', +1 for \0
        if (str == NULL) throw_code_emission_err("malloc failed");
        
        snprintf(str, digits+2, "$%d", op->inner.immediate);
        return str;
    } else if (op->type == OPERAND_REGISTER) {
        if (op->inner.reg == REGISTER_RAX) return "\%eax";
        else if (op->inner.reg == REGISTER_R10) return "\%r10d";
        else throw_code_emission_err("unknown register");
    } else if (op->type == OPERAND_STACK_OFFSET) {
        size_t len = int_digits(op->inner.stack_offset) + strlen("-(%rbp)") + 1;
        char* str = malloc(len);
        if (str == NULL) throw_code_emission_err("malloc failed");
        
        snprintf(str, len, "-%ld(%%rbp)", op->inner.stack_offset);
        return str;
    } else {
        throw_code_emission_err("unknown operand type");
    }
}


char* emit_asm_instruction(AsmInstruction* instr) {
    if (instr->type == INSTRUCTION_MOV) {
        const char template[] = "    movl , \n";
        char* src_str = emit_asm_operand(&instr->inner.mov.src);
        char* dst_str = emit_asm_operand(&instr->inner.mov.dst);

        size_t instruction_len = strlen(template) + strlen(src_str) + strlen(dst_str) + 1;
        char* str = malloc(instruction_len);
        if (str == NULL) throw_code_emission_err("malloc failed");

        snprintf(str, instruction_len, "    movl %s, %s\n", src_str, dst_str);
        return str;
    } else if (instr->type == INSTRUCTION_UNARY) {
        char* template;
        if (instr->inner.unary.op == OPERATOR_NEGATE) template = "    negl %s\n";
        else if (instr->inner.unary.op == OPERATOR_BITWISE_COMPLEMENT) template = "    notl %s\n";
        else throw_code_emission_err("unknown unary operator type");

        char* operand_str = emit_asm_operand(&instr->inner.unary.operand);

        size_t len = strlen(template) + strlen(operand_str) + 1;
        char* str = malloc(len);
        if (str == NULL) throw_code_emission_err("malloc failed");

        snprintf(str, len, template, operand_str);
        return str;
    } else if (instr->type == INSTRUCTION_RET) {
        return "    movq %rbp, %rsp\n    popq %rbp\n    ret\n";        
    } else {
        throw_code_emission_err("unknown instruction type");
    }
}


char* emit_asm_function_definition(AsmFunctionDefinition* func) {
    const char header_template[] = ".globl \n:\n";
    const char prologue_template[] = "    pushq %rbp\n    movq %rsp, %rbp\n    subq $, %rsp\n";

    size_t len = strlen(header_template) + 2*strlen(func->name) + strlen(prologue_template) + int_digits(func->stack_size) + 1;
    
    char* str = malloc(len);
    if (str == NULL) throw_code_emission_err("malloc failed");

    snprintf(str, len, ".globl %s\n%s:\n    pushq %%rbp\n    movq %%rsp, %%rbp\n    subq $%ld, %%rsp\n", func->name, func->name, func->stack_size);

    for (size_t i = 0; i < func->instructions.size; ++i) {
        char* instr = emit_asm_instruction(&func->instructions.instructions[i]);

        str = realloc(str, strlen(str) + strlen(instr) + 1);
        if (str == NULL) throw_code_emission_err("realloc failed");

        strcat(str, instr);
    }

    return str;
}


char* emit_asm_program(AsmProgram* program) {
    char exec_stack[] = ".section .note.GNU-stack,\"\",@progbits\n";
    char* str = emit_asm_function_definition(&program->function_definition);

    str = realloc(str, (strlen(str) + strlen(exec_stack) + 1));
    if (str == NULL) throw_code_emission_err("realloc failed");
    
    strcat(str, exec_stack);
    return str;
}

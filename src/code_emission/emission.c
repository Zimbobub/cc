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
        size_t len = int_digits(op->inner.immediate);

        char* str = malloc(len + 2); // +1 for the '$', +1 for \0
        if (str == NULL) throw_code_emission_err("malloc failed");
        memset(str, 0, len+2);
        
        snprintf(str, len+2, "$%d", op->inner.immediate);
        return str;
    } else if (op->type == OPERAND_REGISTER) {
        return "\%eax";
    } else {
        throw_code_emission_err("unknown operand type");
    }
}


char* emit_asm_instruction(AsmInstruction* instr) {
    if (instr->type == INSTRUCTION_MOV) {
        const char template[] = "    movl , \n";
        char* src_str = emit_asm_operand(&instr->instruction.mov.src);
        char* dst_str = emit_asm_operand(&instr->instruction.mov.dst);

        // printf("OPERANDS %s, %s | %ld %ld\n", src_str, dst_str, strlen(src_str), strlen(dst_str));

        size_t instruction_len = strlen(template) + strlen(src_str) + strlen(dst_str) + 1;
        char* str = malloc(instruction_len);
        if (str == NULL) throw_code_emission_err("malloc failed");

        snprintf(str, instruction_len, "    movl %s, %s\n", src_str, dst_str);
        return str;
    } else if (instr->type == INSTRUCTION_RET) {
        return "    ret\n";        
    } else {
        throw_code_emission_err("unknown instruction type");
    }
}


char* emit_asm_function_definition(AsmFunctionDefinition* func) {
    const char template[] = ".globl \n:\n";

    size_t len = strlen(template) + 2*strlen(func->name) + 1;
    char* str = malloc(len);
    if (str == NULL) throw_code_emission_err("malloc failed");

    snprintf(str, len, ".globl %s\n%s:\n", func->name, func->name);

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

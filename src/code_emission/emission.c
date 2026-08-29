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
    return snprintf(buf, 64, "%d", val);
}


void emit_asm_operand(String* output, AsmOperand* op) {
    if (op->type == OPERAND_IMMEDIATE) {
        size_t digits = int_digits(op->inner.immediate);

        String_push_format(output, digits+1, "$%d", op->inner.immediate);
    } else if (op->type == OPERAND_REGISTER) {
        if (op->inner.reg == REGISTER_RAX) String_push(output, "\%eax");
        else if (op->inner.reg == REGISTER_RDX) String_push(output, "\%edx");
        else if (op->inner.reg == REGISTER_R10) String_push(output, "\%r10d");
        else if (op->inner.reg == REGISTER_R11) String_push(output, "\%r11d");
        else throw_code_emission_err("unknown register");
    } else if (op->type == OPERAND_STACK_OFFSET) {
        size_t len = int_digits(op->inner.stack_offset) + strlen("-(%rbp)") + 1;

        String_push_format(output, len, "-%ld(%%rbp)", op->inner.stack_offset);
    } else {
        throw_code_emission_err("unknown operand type");
    }
}


void emit_asm_instruction(String* output, AsmInstruction* instr) {
    if (instr->type == INSTRUCTION_MOV) {
        String_push(output, "    movl ");
        emit_asm_operand(output, &instr->inner.mov.src);
        String_push(output, ", ");
        emit_asm_operand(output, &instr->inner.mov.dst);
        String_push(output, "\n");
    } else if (instr->type == INSTRUCTION_UNARY) {
        if (instr->inner.unary.op == OPERATOR_NEGATE) String_push(output, "    negl ");
        else if (instr->inner.unary.op == OPERATOR_BITWISE_COMPLEMENT) String_push(output, "    notl ");
        else throw_code_emission_err("unknown unary operator type");

        emit_asm_operand(output, &instr->inner.unary.operand);
        String_push(output, "\n");
    } else if (instr->type == INSTRUCTION_RET) {
        String_push(output, "    movq %rbp, %rsp\n    popq %rbp\n    ret\n");
    } else {
        throw_code_emission_err("unknown instruction type");
    }
}


void emit_asm_function_definition(String* output, AsmFunctionDefinition* func) {
    // function header
    String_push(output, ".globl ");
    String_push(output, func->name);
    String_push(output, "\n");
    String_push(output, func->name);
    String_push(output, ":\n");

    // function prologue
    String_push(output, "    pushq %rbp\n");
    String_push(output, "    movq %rsp, %rbp\n");
    size_t template_len = sizeof("    subq $, %%rsp\n\n");
    String_push_format(output, template_len + int_digits(func->stack_size), "    subq $%ld, %%rsp\n\n", func->stack_size);

    // instructions
    for (size_t i = 0; i < func->instructions.size; ++i) {
        emit_asm_instruction(output, &func->instructions.inner[i]);
    }
}


char* emit_asm_program(AsmProgram* program) {
    String output = String_new(4096);

    String_push(&output, ".section .note.GNU-stack,\"\",@progbits\n");
    emit_asm_function_definition(&output, &program->function_definition);

    return output.ptr;
}

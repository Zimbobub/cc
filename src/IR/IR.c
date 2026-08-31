#include "IR.h"



// PRINTING
void print_IR_value(IRValue* val) {
    if (val->type == IR_VALUE_CONSTANT) printf("const(%d)", val->inner.constant.val);
    else if (val->type == IR_VALUE_VARIABLE) printf("var(%s)", val->inner.var.identifier);
}

void print_IR_instruction(IRInstruction* instr, int depth) {
    if (instr->type == IR_INSTRUCTION_RETURN) {
        printf("%*creturn ", depth, ' ');
        print_IR_value(&instr->inner.ret.val);
        printf("\n");
    } else if (instr->type == IR_INSTRUCTION_UNARY) {
        printf("%*c", depth, ' ');
        print_IR_value(&instr->inner.unary.dst);
        printf(" = %s", operator_to_string(instr->inner.unary.op));
        print_IR_value(&instr->inner.unary.src);
        printf("\n");
    } else if (instr->type == IR_INSTRUCTION_BINARY) {
        printf("%*c", depth, ' ');
        print_IR_value(&instr->inner.binary.dst);
        printf(" = ");
        print_IR_value(&instr->inner.binary.left);
        printf(" %s ", operator_to_string(instr->inner.binary.op));
        print_IR_value(&instr->inner.binary.right);
        printf("\n");
    }
}

void print_IR_function_definition(IRFunctionDefinition* func, int depth) {
    printf("%*cFunctionDefinition (%s) {\n", depth, ' ', func->name);
    for (size_t i = 0; i < func->body.size; i++) {
        print_IR_instruction(&func->body.inner[i], depth+2);
    }
    printf("%*c}\n", depth, ' ');
}

void print_IR_program(IRProgram* program) {
    printf("IRProgram {\n");
    print_IR_function_definition(&program->func, 2);
    printf("}\n");
}


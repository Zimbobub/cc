#include "to_IR.h"



// IR CONVERSION
void throw_IR_err(const char* msg) {
    fprintf(stderr, "AST to IR conversion error: ");
    fprintf(stderr, "%s", msg);
    exit(EXIT_FAILURE);
}

// temp variable names (wont conflict due to counter, and the period which user variables cant use)
// TODO: add function name
size_t temp_ident_index = 0;
const char* make_temp_ident() {
    char* buf = malloc(64);
    if (buf == NULL) throw_IR_err("malloc failed");
    snprintf(buf, 64, "tmp.%ld", temp_ident_index);
    temp_ident_index++;
    return buf;
}

// appends instrucitons to IRInstructions, returns the output value
IRValue IR_transform_expr(CExpression ast, IRInstructions* instructions) {
    if (ast.type == EXPRESSION_CONST) {
        return (IRValue){
            .type=IR_VALUE_CONSTANT,
            .inner.constant.val = ast.expr.constant.val
        };
    } else if (ast.type == EXPRESSION_UNARY) {
        IRValue inner_expr = IR_transform_expr(*ast.expr.unary.val, instructions);

        instructions->inner = realloc(instructions->inner, instructions->size*sizeof(IRInstruction) + sizeof(IRInstruction));
        if (instructions->inner == NULL) throw_IR_err("malloc failed");

        const char* dst = make_temp_ident();

        instructions->inner[instructions->size] = (IRInstruction){
            .type=IR_INSTRUCTION_UNARY,
            .inner.unary = {
                .op=ast.expr.unary.op,
                .src=inner_expr,
                .dst = {
                    .type=IR_VALUE_VARIABLE,
                    .inner.var.identifier=dst
                },
            }
        };

        instructions->size++;

        return (IRValue) {
            .type=IR_VALUE_VARIABLE,
            .inner.var.identifier=dst
        };
    } else if (ast.type == EXPRESSION_BINARY) {
        IRValue left = IR_transform_expr(*ast.expr.binary.left, instructions);
        IRValue right = IR_transform_expr(*ast.expr.binary.right, instructions);

        const char* dst = make_temp_ident();

        instructions->inner = realloc(instructions->inner, instructions->size*sizeof(IRInstruction) + sizeof(IRInstruction));
        if (instructions->inner == NULL) throw_IR_err("malloc failed");

        instructions->inner[instructions->size] = (IRInstruction){
            .type=IR_INSTRUCTION_BINARY,
            .inner.binary = {
                .op=ast.expr.binary.op,
                .left=left,
                .right=right,
                .dst = {
                    .type=IR_VALUE_VARIABLE,
                    .inner.var.identifier=dst
                },
            }
        };

        instructions->size++;

        return (IRValue) {
            .type=IR_VALUE_VARIABLE,
            .inner.var.identifier=dst
        };
    } else {
        throw_IR_err("unknown expr type");
    }
}

IRInstructions IR_transform_instruction(CStatement ast) {
    if (ast.type == STATEMENT_RETURN) {
        IRInstructions instructions = {
            .inner=NULL, // realloc will just call malloc on a nullptr
            .size=0
        };

        IRValue dst = IR_transform_expr(ast.statement.ret, &instructions);

        // alloc 1 more instruction
        instructions.inner = realloc(instructions.inner, instructions.size*sizeof(IRInstruction) + sizeof(IRInstruction));
        if (instructions.inner == NULL) throw_IR_err("realloc failed");

        instructions.inner[instructions.size] = (IRInstruction) {
            .type=IR_INSTRUCTION_RETURN,
            .inner.ret.val = dst
        };
        instructions.size++;
        return instructions;
    } else {
        throw_IR_err("unknown statement type");
    }
}

IRFunctionDefinition IR_transform_func_def(CFunctionDefinition ast) {
    IRFunctionDefinition func = {
        .name=ast.name,
        .body=IR_transform_instruction(ast.body)
    };
    return func;
}

IRProgram IR_transform_ast(CProgram ast) {
    IRProgram prog = {
        .func=IR_transform_func_def(ast.function_definition)
    };
    return prog;
}

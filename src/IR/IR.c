#include "IR.h"



// PRINTING
void print_IR_value(IRValue* val) {
    if (val->type == IR_VALUE_CONSTANT) printf("const(%d)", val->inner.constant.val);
    else if (val->type == IR_VALUE_VARIABLE) printf("var(%s)", val->inner.var.identifier);
}

void print_IR_instruction(IRInstruction* instr, int depth) {
    printf("%*cInstruction {\n", depth, ' ');
    if (instr->type == IR_INSTRUCTION_RETURN) {
        printf("%*creturn ", depth+2);
        print_IR_value(&instr->inner.ret.val);
        printf("\n");
    } else if (instr->type == IR_INSTRUCTION_UNARY) {
        printf("%*c", depth+2);
        print_IR_value(&instr->inner.unary.dst);
        printf(" = %c", instr->inner.unary.op); // op reprs char
        print_IR_value(&instr->inner.unary.src);
        printf("\n");
    }
    printf("%*c}\n", depth, ' ');
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







// IR CONVERSION
void throw_IR_err(const char* msg) {
    fprintf(stderr, "AST to IR conversion error: ");
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}

// temp variable names (wont conflict due to counter, and the period which user variables cant use)
// TODO: add function name
size_t temp_ident_index = 0;
const char* make_temp_ident() {
    char buf[64];
    snprintf(buf, 64, "tmp.%d", temp_ident_index);
    return buf;
}

// appends instrucitons to IRInstructions, returns the output value
IRValue transform_expr(CExpression ast, IRInstructions* instructions) {
    if (ast.type == EXPRESSION_CONST) {
        return (IRValue){
            .type=IR_VALUE_CONSTANT,
            .inner.constant.val = ast.expr.constant.val
        };
    } else if (ast.type == EXPRESSION_UNARY) {
        IRValue inner_expr = transform_expr(*ast.expr.unary.val, instructions);

        IRInstruction* instr = malloc(sizeof(IRInstruction));
        if (instr == NULL) throw_IR_err("malloc failed");

        const char* dst = make_temp_ident();

        *instr = (IRInstruction){
            .type=IR_INSTRUCTION_UNARY,
            .inner.unary = {
                .op=ast.expr.unary.op,
                .src=inner_expr,
                .dst=dst,
            }
        };

        return (IRValue) {
            .type=IR_VALUE_VARIABLE,
            .inner.var.identifier=dst
        };
    } else if (ast.type == EXPRESSION_BINARY) {
        throw_IR_err("TODO");
    } else {
        throw_IR_err("unknown expr type");
    }
}

IRInstructions transform_statement(CStatement ast) {
    if (ast.type == STATEMENT_RETURN) {
        IRInstructions instructions = {
            .inner=NULL, // realloc will just call malloc on a nullptr
            .size=0
        };

        IRValue dst = transform_expr(ast.statement.ret, &instructions);

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

IRFunctionDefinition transform_func_def(CFunctionDefinition ast) {
    IRFunctionDefinition func = {
        .name=ast.name,
        .body=transform_statement(ast.body)
    };
    return func;
}

IRProgram transform_ast(CProgram ast) {
    IRProgram prog = {
        .func=transform_func_def(ast.function_definition)
    };
    return prog;
}
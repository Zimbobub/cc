#include "codegen.h"


void throw_err(const char* msg) {
    printf("Codegen error\n");
    printf("%s\n", msg);
    exit(1);
}


AsmInstructions transform_expression(CExpression ast) {
    if (ast.type == EXPRESSION_CONST) {
        AsmInstructions instructions = {
            .instructions=malloc(sizeof(AsmInstruction)),
            .size=1
        };
        if (instructions.instructions == NULL) throw_err("Malloc failed");

        AsmInstruction instr = {
            .type=INSTRUCTION_MOV,
            .instruction.mov = {
                .dst.type=OPERAND_REGISTER,
                .src.type=OPERAND_IMMEDIATE,
                .src.operand.immediate = ast.expr.constant.val
            },
        };
        instructions.instructions[0] = instr;
    } else {
        throw_err("Unknown expression type");
    }
}

AsmInstructions transform_statement(CStatement ast) {
    if (ast.type == STATEMENT_RETURN) {
        AsmInstructions instructions = {
            .instructions=malloc(2*sizeof(AsmInstruction)),
            .size=2
        };
        if (instructions.instructions == NULL) throw_err("Malloc failed");

        // mov <val> <reg>
        instructions.instructions[0] = (AsmInstruction) {
            .type=INSTRUCTION_MOV,
            .instruction.mov={
                .dst={
                    .type=OPERAND_REGISTER
                },
                .src={
                    .type=OPERAND_IMMEDIATE,
                    .operand.immediate = ast.statement.ret.expr.constant.val
                }
            }
        };

        // ret
        instructions.instructions[1] = (AsmInstruction) {
            .type=INSTRUCTION_RET
        };
        return instructions;
    } else {
        // throw err
    }
}

AsmFunctionDefinition transform_func(CFunctionDefinition ast) {
    AsmFunctionDefinition func = {
        .name=ast.name
    };
    return func;
}

AsmProgram transform_program(CProgram ast) {
    AsmProgram program = {
        transform_func(ast.function_definition)
    };
    return program;
}
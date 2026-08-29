#include "codegen.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_codegen_err(const char* msg) {
    printf("Codegen error\n");
    printf("%s\n", msg);
    exit(1);
}


AsmOperand transform_operand(IRValue ir) {
    if (ir.type == IR_VALUE_CONSTANT) {
        return (AsmOperand) {
            .type=OPERAND_IMMEDIATE,
            .inner.immediate=ir.inner.constant.val
        };
    } else if (ir.type == IR_VALUE_VARIABLE) {
        return (AsmOperand) {
            .type=OPERAND_PSEUDO,
            .inner.pseudo=ir.inner.var.identifier
        };
    } else {
        throw_codegen_err("Unknown IRValue type");
    }
}

AsmInstructions transform_instruction(IRInstruction ir) {
    if (ir.type == IR_INSTRUCTION_RETURN) {
        AsmInstructions instructions = {
            .instructions=malloc(2*sizeof(AsmInstruction)),
            .size=2
        };
        if (instructions.instructions == NULL) throw_codegen_err("Malloc failed");

        // mov <val> to RAX
        instructions.instructions[0] = (AsmInstruction) {
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=transform_operand(ir.inner.ret.val),
                .dst={
                    .type=OPERAND_REGISTER,
                    .inner.reg=REGISTER_RAX
                }
            }
        };

        // ret
        instructions.instructions[1] = (AsmInstruction) {
            .type=INSTRUCTION_RET
        };
        return instructions;
    } else if (ir.type == IR_INSTRUCTION_UNARY) {
        AsmInstructions instructions = {
            .instructions=malloc(2*sizeof(AsmInstruction)),
            .size=2
        };
        if (instructions.instructions == NULL) throw_codegen_err("Malloc failed");

        // mov src to dst
        instructions.instructions[0] = (AsmInstruction) {
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=transform_operand(ir.inner.unary.src),
                .dst=transform_operand(ir.inner.unary.dst)
            }
        };

        // operate on dst
        instructions.instructions[1] = (AsmInstruction) {
            .type=INSTRUCTION_UNARY,
            .inner.unary= {
                .op=ir.inner.unary.op,
                .operand=transform_operand(ir.inner.unary.dst)
            }
        };
        return instructions;
    } else if (ir.type == IR_INSTRUCTION_BINARY) {
        if (ir.inner.binary.op == OPERATOR_ADD || ir.inner.binary.op == OPERATOR_SUB || ir.inner.binary.op == OPERATOR_MUL) {
            // dst = left + right
            // becomes
            // dst = left
            // dst += right
            AsmInstructions instructions = {
                .instructions=malloc(2*sizeof(AsmInstruction)),
                .size=2
            };
            if (instructions.instructions == NULL) throw_codegen_err("Malloc failed");
    
            // mov src to dst
            instructions.instructions[0] = (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src=transform_operand(ir.inner.binary.left),
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            };
    
            // operate on dst
            instructions.instructions[1] = (AsmInstruction) {
                .type=INSTRUCTION_BINARY,
                .inner.binary={
                    .op=ir.inner.binary.op,
                    .src=transform_operand(ir.inner.binary.right),
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            };
            return instructions;
        } else if (ir.inner.binary.op == OPERATOR_DIV || ir.inner.binary.op == OPERATOR_MOD) {
            // dst = left / right
            // becomes
            // EAX = left
            // EDX = sign_extend(EAX)
            // idiv right
            // dst = EAX (or for mod, dst = EDX)
            AsmInstructions instructions = {
                .instructions=malloc(4*sizeof(AsmInstruction)),
                .size=2
            };
            if (instructions.instructions == NULL) throw_codegen_err("Malloc failed");
    
            // EAX = left
            instructions.instructions[0] = (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src=transform_operand(ir.inner.binary.left),
                    .dst={
                        .type=OPERAND_REGISTER,
                        .inner.reg=REGISTER_RAX
                    }
                }
            };
    
            // EDX = sign_extend(EAX)
            instructions.instructions[1] = (AsmInstruction) {
                .type=INSTRUCTION_CDQ,
            };

            // idiv right
            instructions.instructions[2] = (AsmInstruction) {
                .type=INSTRUCTION_IDIV,
                .inner.idiv=transform_operand(ir.inner.binary.right)
            };

            // dst = EAX (for div) | EDX (for mod)
            instructions.instructions[3] = (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src={
                        .type=OPERAND_REGISTER,
                        .inner.reg=(ir.inner.binary.op == OPERATOR_DIV ? REGISTER_RAX : REGISTER_RDX)
                    },
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            };
            return instructions;
        } else {
            throw_codegen_err("unknown binary operand");
        }
    } else {
        throw_codegen_err("unknown IRInstruction type");
    }
}

AsmFunctionDefinition transform_func(IRFunctionDefinition ir) {
    // TODO: allow multiple statements
    AsmInstructions instructions = {
        .instructions=NULL,
        .size=0
    };
    
    for (size_t i = 0; i < ir.body.size; ++i) {
        AsmInstructions add = transform_instruction(ir.body.inner[i]);
        instructions.size += add.size;
        instructions.instructions = realloc(instructions.instructions, instructions.size * sizeof(AsmInstruction));
        if (instructions.instructions == NULL) throw_codegen_err("realloc failed");

        for (size_t j = 0; j < add.size; j++) {
            // instructions.instructions[instructions.size-add.size-1] = add.instructions[j];
            memcpy(&instructions.instructions[instructions.size - add.size + j], &add.instructions[j], sizeof(AsmInstruction));
        }
        free(add.instructions);
    }

    AsmFunctionDefinition func = {
        .name=ir.name,
        .instructions=instructions
    };
    return func;
}

AsmProgram transform_program(IRProgram ir) {
    AsmProgram program = {
        transform_func(ir.func)
    };
    return program;
}
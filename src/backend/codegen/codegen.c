#include "codegen.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_codegen_err(const char* msg) {
    printf("Codegen error\n");
    printf("%s\n", msg);
    exit(1);
}


AsmOperand transform_operand(IRValue ir) {
    if (ir.type == IR_VALUE_CONSTANT) return AsmOperand_imm(ir.inner.constant.val);
    else if (ir.type == IR_VALUE_VARIABLE) return AsmOperand_pseudo(ir.inner.var.identifier);
    else throw_codegen_err("Unknown IRValue type");
}

AsmInstructions transform_instruction(IRInstruction ir) {
    AsmInstructions instructions = {0};

    if (ir.type == IR_INSTRUCTION_RETURN) {
        // mov <val> to RAX
        AsmInstructions_push(&instructions, (AsmInstruction) {
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=transform_operand(ir.inner.ret.val),
                .dst=AsmOperand_reg(REGISTER_RAX)
            }
        });

        AsmInstructions_push(&instructions, (AsmInstruction) {
            .type=INSTRUCTION_RET,
        });
    } else if (ir.type == IR_INSTRUCTION_UNARY) {
        // mov src to dst
        AsmInstructions_push(&instructions, (AsmInstruction) {
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=transform_operand(ir.inner.unary.src),
                .dst=transform_operand(ir.inner.unary.dst)
            }
        });

        // operate on dst
        AsmInstructions_push(&instructions, (AsmInstruction) {
            .type=INSTRUCTION_UNARY,
            .inner.unary= {
                .op=ir.inner.unary.op,
                .operand=transform_operand(ir.inner.unary.dst)
            }
        });
    } else if (ir.type == IR_INSTRUCTION_BINARY) {
        if (ir.inner.binary.op == OPERATOR_ADD || ir.inner.binary.op == OPERATOR_SUB || ir.inner.binary.op == OPERATOR_MUL) {
            // dst = left + right
            // becomes
            // dst = left
            // dst += right

            // mov src to dst
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src=transform_operand(ir.inner.binary.left),
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            });
    
            // operate on dst
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_BINARY,
                .inner.binary={
                    .op=ir.inner.binary.op,
                    .src=transform_operand(ir.inner.binary.right),
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            });
        } else if (ir.inner.binary.op == OPERATOR_DIV || ir.inner.binary.op == OPERATOR_MOD) {
            // dst = left / right
            // becomes
            // EAX = left
            // EDX = sign_extend(EAX)
            // idiv right
            // dst = EAX (or for mod, dst = EDX)

            // EAX = left
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src=transform_operand(ir.inner.binary.left),
                    .dst={
                        .type=OPERAND_REGISTER,
                        .inner.reg=REGISTER_RAX
                    }
                }
            });
    
            // EDX = sign_extend(EAX)
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_CDQ,
            });

            // idiv right
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_IDIV,
                .inner.idiv=transform_operand(ir.inner.binary.right)
            });

            // dst = EAX (for div) | EDX (for mod)
            AsmInstructions_push(&instructions, (AsmInstruction) {
                .type=INSTRUCTION_MOV,
                .inner.mov={
                    .src=(ir.inner.binary.op == OPERATOR_DIV ? AsmOperand_reg(REGISTER_RAX) : AsmOperand_reg(REGISTER_RDX)),
                    .dst=transform_operand(ir.inner.binary.dst)
                }
            });
        } else {
            throw_codegen_err("unknown binary operand");
        }
    } else {
        throw_codegen_err("unknown IRInstruction type");
    }

    return instructions;
}

AsmFunctionDefinition transform_func(IRFunctionDefinition ir) {
    AsmInstructions instructions = {
        .inner=NULL,
        .size=0
    };
    
    for (size_t i = 0; i < ir.body.size; ++i) {
        AsmInstructions add = transform_instruction(ir.body.inner[i]);
        instructions = *AsmInstructions_append(&instructions, &add);
        free(add.inner);
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
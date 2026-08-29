#include "./fixup_instructions.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_asm_fixup_err(const char* msg) {
    printf("Asm cleanup error\n");
    printf("%s\n", msg);
    exit(1);
}



void fixup_mov(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_MOV) throw_asm_fixup_err("fixup_mov() called on non-mov instruction");
    if (instr.inner.mov.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("mov dst is an immediate");

    if (instr.inner.mov.src.type == OPERAND_STACK_OFFSET && instr.inner.mov.dst.type == OPERAND_STACK_OFFSET) {
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=instr.inner.mov.src,
                .dst=AsmOperand_reg(REGISTER_R10)
            }
        });

        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=AsmOperand_reg(REGISTER_R10),
                .dst=instr.inner.mov.dst
            }
        });
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


// TODO FINISH
void fixup_add_sub(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_BINARY) throw_asm_fixup_err("fixup_binary() called on non-binary instruction");
    if (instr.inner.binary.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("operation dst is an immediate");

    if (instr.inner.binary.src.type == OPERAND_STACK_OFFSET && instr.inner.binary.dst.type == OPERAND_STACK_OFFSET) {
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov={
                .src=instr.inner.binary.src,
                .dst=AsmOperand_reg(REGISTER_R10)
            }
        });

        instr.inner.binary.src = AsmOperand_reg(REGISTER_R10);
        AsmInstructions_push(fixed_instructions, instr);
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


// imul cant have a memory address as its dst
void fixup_imul(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    // TODO
}


void fixup_instruction(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type == INSTRUCTION_MOV) fixup_mov(instr, fixed_instructions);
    if (instr.type == INSTRUCTION_BINARY) {
        if (instr.inner.binary.op == OPERATOR_MUL) fixup_imul(instr, fixed_instructions);
        else fixup_add_sub(instr, fixed_instructions);
    }
}




AsmFunctionDefinition fixup_function(AsmFunctionDefinition* func) {
    AsmInstructions instructions = {
        .inner=NULL,
        .size=0
    };
    
    for (size_t i = 0; i < func->instructions.size; ++i) {
        fixup_instruction(func->instructions.inner[i], &instructions);
    }

    AsmFunctionDefinition out = {
        .name=func->name,
        .stack_size=func->stack_size,
        .instructions=instructions
    };
    return out;
}


void fixup_program(AsmProgram* program) {
    program->function_definition = fixup_function(&program->function_definition);
}


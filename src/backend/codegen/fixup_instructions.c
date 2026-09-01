#include "./fixup_instructions.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_asm_fixup_err(const char* msg) {
    printf("Asm cleanup error\n");
    printf("%s\n", msg);
    exit(1);
}


// Fix move from memory to memory
void fixup_mov(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_MOV) throw_asm_fixup_err("fixup_mov() called on non-mov instruction");
    if (instr.inner.mov.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("mov dst is an immediate");

    if (instr.inner.mov.src.type == OPERAND_STACK_OFFSET && instr.inner.mov.dst.type == OPERAND_STACK_OFFSET) {
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=instr.inner.mov.src,
            .inner.mov.dst=AsmOperand_reg(REGISTER_R10)
        });

        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=AsmOperand_reg(REGISTER_R10),
            .inner.mov.dst=instr.inner.mov.dst
        });
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


// fix add or sub memory to memory
void fixup_binop_default(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_BINARY) throw_asm_fixup_err("fixup_binop_default() called on non-binary instruction");
    // if (instr.inner.binary.op != OPERATOR_ADD && instr.inner.binary.op != OPERATOR_SUB) throw_asm_fixup_err("fixup_binop_default() called on binop that isnt add or sub");
    if (instr.inner.binary.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("operation dst is an immediate");

    if (instr.inner.binary.src.type == OPERAND_STACK_OFFSET && instr.inner.binary.dst.type == OPERAND_STACK_OFFSET) {
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=instr.inner.binary.src,
            .inner.mov.dst=AsmOperand_reg(REGISTER_R10)
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
    if (instr.type != INSTRUCTION_BINARY) throw_asm_fixup_err("fixup_imul() called on non-binary instruction");
    if (instr.inner.binary.op != OPERATOR_MUL) throw_asm_fixup_err("fixup_imul() called on binop that isnt mul");
    if (instr.inner.binary.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("operation dst is an immediate");

    if (instr.inner.binary.dst.type == OPERAND_STACK_OFFSET) {
        // imul src, dst (dst *= src), where dst is a memory addr
        // becomes
        // mov dst to r11
        // imul src r11
        // mov r11 to dst

        // mov dst to r11
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=instr.inner.binary.dst,
            .inner.mov.dst=AsmOperand_reg(REGISTER_R11)
        });

        // imul src r11
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_BINARY,
            .inner.binary.op=OPERATOR_MUL,
            .inner.binary.src=instr.inner.binary.src,
            .inner.binary.dst=AsmOperand_reg(REGISTER_R11)
        });

        // mov r11 to dst
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=AsmOperand_reg(REGISTER_R11),
            .inner.mov.dst=instr.inner.binary.dst
        });
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


// shift amount must be an immediate, or in register ECX
void fixup_shift(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_SHIFT) throw_asm_fixup_err("fixup_shift() called on non-shift instruction");
    if (instr.inner.shift.operand.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("operation dst is an immediate");

    if (
        instr.inner.shift.shift_amount.type == OPERAND_REGISTER
     || instr.inner.shift.shift_amount.type == OPERAND_STACK_OFFSET
    ) {
        // shift dst, amt
        // becomes
        // mov ecx to r10
        // mov amt to ecx
        // shift dst ecx
        // mov r10 to ecx

        // save ecx
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=AsmOperand_reg(REGISTER_RCX),
            .inner.mov.dst=AsmOperand_reg(REGISTER_R10)
        });

        // mov amt to ecx
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=instr.inner.shift.shift_amount,
            .inner.mov.dst=AsmOperand_reg(REGISTER_RCX)
        });

        // shift dst by amt
        instr.inner.shift.shift_amount = AsmOperand_reg(REGISTER_RCX);
        AsmInstructions_push(fixed_instructions, instr);

        // restore ecx
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=AsmOperand_reg(REGISTER_R10),
            .inner.mov.dst=AsmOperand_reg(REGISTER_RCX)
        });
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


// idiv cant be called using an immediate
void fixup_idiv(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_IDIV) throw_asm_fixup_err("fixup_idiv() called on non-idiv instruction");

    if (instr.inner.idiv.type == OPERAND_IMMEDIATE) {
        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_MOV,
            .inner.mov.src=instr.inner.idiv,
            .inner.mov.dst=AsmOperand_reg(REGISTER_R10)
        });

        AsmInstructions_push(fixed_instructions, (AsmInstruction){
            .type=INSTRUCTION_IDIV,
            .inner.idiv=AsmOperand_reg(REGISTER_R10)
        });
    } else {
        // no problem detected
        AsmInstructions_push(fixed_instructions, instr);
    }
}


void fixup_instruction(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type == INSTRUCTION_MOV) fixup_mov(instr, fixed_instructions);
    else if (instr.type == INSTRUCTION_BINARY) {
        if (instr.inner.binary.op == OPERATOR_MUL) fixup_imul(instr, fixed_instructions);
        else if (instr.inner.binary.op == OPERATOR_MUL) fixup_shift(instr, fixed_instructions);
        else fixup_binop_default(instr, fixed_instructions);
    } else if (instr.type == INSTRUCTION_IDIV) fixup_idiv(instr, fixed_instructions);
    else if (instr.type == INSTRUCTION_SHIFT) fixup_shift(instr, fixed_instructions);
    else {
        AsmInstructions_push(fixed_instructions, instr);
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


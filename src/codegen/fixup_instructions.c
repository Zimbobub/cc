#include "./fixup_instructions.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_asm_fixup_err(const char* msg) {
    printf("Asm cleanup error\n");
    printf("%s\n", msg);
    exit(1);
}

// void replace_memory_to_memory_instruction(AsmFunctionDefinition* this, size_t index) {
//     this->instructions.size++;
//     this->instructions.inner = realloc(this->instructions.inner, this->instructions.size * sizeof(AsmInstruction));
//     if (this->instructions.inner == NULL) throw_asm_fixup_err("realloc failed");

//     // shift all following instructions forward 1
//     // also copy our instructions to the next index
//     for (size_t i = this->instructions.size-1; i > index; i--) {
//         memcpy(&this->instructions.inner[i], &this->instructions.inner[i-1], sizeof(AsmInstruction));
//     }

//     // add intemediary reg
//     if (this->instructions.inner[index].type == INSTRUCTION_MOV) {
//         this->instructions.inner[index].inner.mov.dst = (AsmOperand) {
//             .type=OPERAND_REGISTER,
//             .inner.reg=REGISTER_R10
//         };

//         this->instructions.inner[index+1].inner.mov.src = (AsmOperand) {
//             .type=OPERAND_REGISTER,
//             .inner.reg=REGISTER_R10
//         };
//     } else {
//         throw_asm_fixup_err("replace_memory_to_memory_instruction should not be called on an instruction that doesnt use 2 operands");
//     }
// }

// void replace_memory_to_memory_instructions(AsmProgram* this) {
//     for (size_t i = 0; i < this->function_definition.instructions.size; i++) {
//         if (this->function_definition.instructions.inner[i].type == INSTRUCTION_MOV) {
//             if (this->function_definition.instructions.inner[i].inner.mov.src.type == OPERAND_STACK_OFFSET &&
//                 this->function_definition.instructions.inner[i].inner.mov.dst.type == OPERAND_STACK_OFFSET) {
//                 replace_memory_to_memory_instruction(&this->function_definition, i);
//             }
//         }
//     }
// }



void fixup_mov(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_MOV) throw_asm_fixup_err("fixup_mov() called on non-mov instruction");

    if (instr.inner.mov.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("mov dst is an immediate");

    if (instr.inner.mov.src.type == OPERAND_STACK_OFFSET && instr.inner.mov.dst.type == OPERAND_STACK_OFFSET) {
        fixed_instructions->size += 2;
        fixed_instructions->inner = realloc(fixed_instructions->inner, fixed_instructions->size * sizeof(AsmInstruction));
        if (fixed_instructions->inner == NULL) throw_asm_fixup_err("realloc failed");

        // copy original instruction twice
        memcpy(&fixed_instructions->inner[fixed_instructions->size-2], &instr, sizeof(AsmInstruction));
        memcpy(&fixed_instructions->inner[fixed_instructions->size-1], &instr, sizeof(AsmInstruction));

        // then add an intermediary register
        fixed_instructions->inner[fixed_instructions->size-2].inner.mov.dst = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };

        fixed_instructions->inner[fixed_instructions->size-1].inner.mov.src = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };
    }

    // no problem detected
    fixed_instructions->size += 1;
    fixed_instructions->inner = realloc(fixed_instructions->inner, fixed_instructions->size * sizeof(AsmInstruction));
    if (fixed_instructions->inner == NULL) throw_asm_fixup_err("realloc failed");

    // copy original instruction
    memcpy(&fixed_instructions->inner[fixed_instructions->size-1], &instr, sizeof(AsmInstruction));
}


// imul cant have a memory address as its dst
void fixup_imul(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    // TODO
}


// TODO FINISH
void fixup_add_sub(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type != INSTRUCTION_BINARY) throw_asm_fixup_err("fixup_binary() called on non-binary instruction");

    if (instr.inner.binary.dst.type == OPERAND_IMMEDIATE) throw_asm_fixup_err("operation dst is an immediate");

    if (instr.inner.binary.src.type == OPERAND_STACK_OFFSET && instr.inner.binary.dst.type == OPERAND_STACK_OFFSET) {
        fixed_instructions->size += 2;
        fixed_instructions->inner = realloc(fixed_instructions->inner, fixed_instructions->size * sizeof(AsmInstruction));
        if (fixed_instructions->inner == NULL) throw_asm_fixup_err("realloc failed");

        // copy original instruction twice
        memcpy(&fixed_instructions->inner[fixed_instructions->size-2], &instr, sizeof(AsmInstruction));
        memcpy(&fixed_instructions->inner[fixed_instructions->size-1], &instr, sizeof(AsmInstruction));

        // then add an intermediary register
        fixed_instructions->inner[fixed_instructions->size-2].inner.mov.dst = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };

        fixed_instructions->inner[fixed_instructions->size-1].inner.mov.src = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };
    }

    // no problem detected
    fixed_instructions->size += 1;
    fixed_instructions->inner = realloc(fixed_instructions->inner, fixed_instructions->size * sizeof(AsmInstruction));
    if (fixed_instructions->inner == NULL) throw_asm_fixup_err("realloc failed");

    // copy original instruction
    memcpy(&fixed_instructions->inner[fixed_instructions->size-1], &instr, sizeof(AsmInstruction));
}


void fixup_instruction(AsmInstruction instr, AsmInstructions* fixed_instructions) {
    if (instr.type == INSTRUCTION_MOV) fixup_mov(instr, fixed_instructions);
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


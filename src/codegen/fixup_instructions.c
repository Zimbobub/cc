#include "./fixup_instructions.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_asm_fixup_err(const char* msg) {
    printf("Asm cleanup error\n");
    printf("%s\n", msg);
    exit(1);
}

void replace_memory_to_memory_instruction(AsmFunctionDefinition* this, size_t index) {
    this->instructions.size++;
    this->instructions.instructions = realloc(this->instructions.instructions, this->instructions.size * sizeof(AsmInstruction));
    if (this->instructions.instructions == NULL) throw_asm_fixup_err("realloc failed");

    // shift all following instructions forward 1
    // also copy our instructions to the next index
    for (size_t i = this->instructions.size-1; i > index; i--) {
        memcpy(&this->instructions.instructions[i], &this->instructions.instructions[i-1], sizeof(AsmInstruction));
    }

    // add intemediary reg
    if (this->instructions.instructions[index].type == INSTRUCTION_MOV) {
        this->instructions.instructions[index].inner.mov.dst = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };

        this->instructions.instructions[index+1].inner.mov.src = (AsmOperand) {
            .type=OPERAND_REGISTER,
            .inner.reg=REGISTER_R10
        };
    } else {
        throw_asm_fixup_err("replace_memory_to_memory_instruction should not be called on an instruction that doesnt use 2 operands");
    }
}

void replace_memory_to_memory_instructions(AsmProgram* this) {
    for (size_t i = 0; i < this->function_definition.instructions.size; i++) {
        if (this->function_definition.instructions.instructions[i].type == INSTRUCTION_MOV) {
            if (this->function_definition.instructions.instructions[i].inner.mov.src.type == OPERAND_STACK_OFFSET &&
                this->function_definition.instructions.instructions[i].inner.mov.dst.type == OPERAND_STACK_OFFSET) {
                replace_memory_to_memory_instruction(&this->function_definition, i);
            }
        }
    }
}
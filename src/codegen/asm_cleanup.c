#include "./asm_cleanup.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_asm_cleanup_err(const char* msg) {
    printf("Asm cleanup error\n");
    printf("%s\n", msg);
    exit(1);
}

// if not included in identifiers, add it to the list
// identifiers is ***char cos its an array of strings we pass by reference
size_t get_stack_offset(const char* pseudo, char*** identifiers, size_t* n) {
    for (size_t i = 0; i < *n; i++) {
        if (strcmp(pseudo, (*identifiers)[i]) == 0) return VARIABLE_SIZE * (i+1);
    }
    
    // not found
    (*n)++;
    *identifiers = realloc(*identifiers, *n * sizeof(char*));
    if (*identifiers == NULL) throw_asm_cleanup_err("realloc failed");

    (*identifiers)[(*n)-1] = malloc(strlen(pseudo)+1);
    if ((*identifiers)[(*n)-1] == NULL) throw_asm_cleanup_err("malloc failed");

    strcpy((*identifiers)[(*n)-1], pseudo);

    return VARIABLE_SIZE * (*n);
}

// return stack size increase
size_t replace_pseudo_register_operand(AsmOperand* this, char*** identifiers, size_t* num_identifiers) {
    if (this->type == OPERAND_PSEUDO) {
        size_t prev_len = *num_identifiers;
        this->type = OPERAND_STACK_OFFSET;
        this->inner.stack_offset = get_stack_offset(
            this->inner.pseudo, identifiers, num_identifiers
        );

        return (prev_len != *num_identifiers) ? VARIABLE_SIZE : 0;
    }

    return 0;
}

void replace_pseudo_registers_func(AsmFunctionDefinition* this) {
    // map identifier to offsets
    char** identifiers = NULL;
    size_t num_identifiers = 0;

    for (size_t i = 0; i < this->instructions.size; i++) {
        if (this->instructions.instructions[i].type == INSTRUCTION_MOV) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.instructions[i].inner.mov.src, &identifiers, &num_identifiers);
            this->stack_size += replace_pseudo_register_operand(&this->instructions.instructions[i].inner.mov.dst, &identifiers, &num_identifiers);
        } else if (this->instructions.instructions[i].type == INSTRUCTION_UNARY) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.instructions[i].inner.unary.operand, &identifiers, &num_identifiers);
        }
    }
}


// pseudo reg -> stack offset
void replace_pseudo_registers(AsmProgram* this) {
    // TODO: loop over every function
    replace_pseudo_registers_func(&this->function_definition);
}


void replace_memory_to_memory_instruction(AsmFunctionDefinition* this, size_t index) {
    this->instructions.size++;
    this->instructions.instructions = realloc(this->instructions.instructions, this->instructions.size * sizeof(AsmInstruction));
    if (this->instructions.instructions == NULL) throw_asm_cleanup_err("realloc failed");

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
        throw_asm_cleanup_err("replace_memory_to_memory_instruction should not be called on an instruction that doesnt use 2 operands");
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
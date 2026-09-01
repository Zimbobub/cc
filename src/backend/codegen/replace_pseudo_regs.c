#include "replace_pseudo_regs.h"

// dont need to do pretty printing
// if these errors occur, theyre not the user's fault
void throw_pseudo_replace_err(const char* msg) {
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
    if (*identifiers == NULL) throw_pseudo_replace_err("realloc failed");

    (*identifiers)[(*n)-1] = malloc(strlen(pseudo)+1);
    if ((*identifiers)[(*n)-1] == NULL) throw_pseudo_replace_err("malloc failed");

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
        if (this->instructions.inner[i].type == INSTRUCTION_MOV) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.mov.src, &identifiers, &num_identifiers);
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.mov.dst, &identifiers, &num_identifiers);
        } else if (this->instructions.inner[i].type == INSTRUCTION_UNARY) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.unary.operand, &identifiers, &num_identifiers);
        } else if (this->instructions.inner[i].type == INSTRUCTION_BINARY) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.binary.src, &identifiers, &num_identifiers);
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.binary.dst, &identifiers, &num_identifiers);
        } else if (this->instructions.inner[i].type == INSTRUCTION_IDIV) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.idiv, &identifiers, &num_identifiers);
        } else if (this->instructions.inner[i].type == INSTRUCTION_CDQ) {
            // do nothing
        } else if (this->instructions.inner[i].type == INSTRUCTION_SHIFT) {
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.shift.operand, &identifiers, &num_identifiers);
            this->stack_size += replace_pseudo_register_operand(&this->instructions.inner[i].inner.shift.shift_amount, &identifiers, &num_identifiers);
        } else if (this->instructions.inner[i].type == INSTRUCTION_RET) {
            // do nothing
        } else {
            throw_pseudo_replace_err("unknown instruction type");
        }
    }
}


// pseudo reg -> stack offset
void replace_pseudo_registers(AsmProgram* this) {
    // TODO: loop over every function
    replace_pseudo_registers_func(&this->function_definition);
}

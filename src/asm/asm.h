#ifndef ASM_H
#define ASM_H

#include <stddef.h>

// OPERAND
typedef enum {
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER
} AsmOperandType;

typedef struct {
    AsmOperandType type;
    union {
        int immediate;
    } operand;
} AsmOperand;



// INSTRUCTION
typedef enum {
    INSTRUCTION_MOV,
    INSTRUCTION_RET
} AsmInstructionType;

typedef struct {
    AsmInstructionType type;
    union {
        struct {
            AsmOperand src;
            AsmOperand dst;
        } mov;
    } instruction;
} AsmInstruction;


// PROGRAM
typedef struct {
    char* name;
    AsmInstruction* instructions;
    size_t n_instructions;
} AsmFunctionDefinition;


typedef struct {
    AsmFunctionDefinition function_definition;
} AsmProgram;


#endif
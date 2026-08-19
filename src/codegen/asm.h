#ifndef ASM_H
#define ASM_H

#include <stddef.h>

#include "../parser/ast.h"

// OPERAND
typedef enum {
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_PSEUDO,
    OPERAND_STACK_OFFSET
} AsmOperandType;

typedef enum {
    REGISTER_RAX,
    REGISTER_R10
} AsmRegister;

typedef struct {
    AsmOperandType type;
    union {
        int immediate;
        AsmRegister reg;
        const char* pseudo;
        size_t stack_offset;
    } inner;
} AsmOperand;



// INSTRUCTION
typedef enum {
    INSTRUCTION_MOV,
    INSTRUCTION_UNARY,
    INSTRUCTION_RET
} AsmInstructionType;

typedef struct {
    AsmInstructionType type;
    union {
        struct {
            AsmOperand src;
            AsmOperand dst;
        } mov;
        struct {
            UnaryOperator op;
            AsmOperand operand;
        } unary;
    } instruction;
} AsmInstruction;


typedef struct {
    AsmInstruction* instructions;
    size_t size;
} AsmInstructions;


// PROGRAM
typedef struct {
    char* name;
    AsmInstructions instructions;
    size_t stack_size; // used to generate stack setup and cleanup
} AsmFunctionDefinition;


typedef struct {
    AsmFunctionDefinition function_definition;
} AsmProgram;


void print_asm_operand(AsmOperand* op);
void print_asm_instruction(AsmInstruction* instr, int depth);
void print_asm_function_definition(AsmFunctionDefinition* func, int depth);
void print_asm_program(AsmProgram* program);


#endif
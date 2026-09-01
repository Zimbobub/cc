#ifndef ASM_H
#define ASM_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../../frontend/parser/ast.h"

// OPERAND
typedef enum {
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_PSEUDO,
    OPERAND_STACK_OFFSET
} AsmOperandType;

typedef enum {
    REGISTER_RAX,
    REGISTER_RCX,
    REGISTER_RDX,
    REGISTER_R10,
    REGISTER_R11,
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
    INSTRUCTION_UNARY,  // negate / complement
    INSTRUCTION_BINARY, // add / sub / imul
    INSTRUCTION_IDIV,   // idivl x: EAX = (EDX EAX) / x; EDX = (EDX EAX) % x;
    INSTRUCTION_CDQ,    // sign extend EAX into EDX
    INSTRUCTION_SHIFT,
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
        struct {
            BinaryOperator op;
            AsmOperand src;
            AsmOperand dst;
        } binary;
        AsmOperand idiv;
        struct {
            bool is_right;
            AsmOperand operand;
            AsmOperand shift_amount; // can either be an immediate or CL (lowest 8 bits of ECX)
        } shift;
    } inner;
} AsmInstruction;


typedef struct {
    AsmInstruction* inner;
    size_t size;
} AsmInstructions;


// PROGRAM
typedef struct {
    const char* name;
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

AsmOperand AsmOperand_imm(int val);
AsmOperand AsmOperand_reg(AsmRegister reg);
AsmOperand AsmOperand_pseudo(const char* name);
AsmOperand AsmOperand_stack(size_t offset);

void AsmInstructions_push(AsmInstructions* instructions, AsmInstruction instr);
AsmInstructions* AsmInstructions_append(AsmInstructions* instructions, AsmInstructions* other);

#endif
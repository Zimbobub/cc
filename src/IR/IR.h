#ifndef IR_H
#define IR_H

#include <stddef.h>
#include <stdlib.h>

#include "../frontend/parser/ast.h"

// VAL
typedef enum {
    IR_VALUE_CONSTANT,
    IR_VALUE_VARIABLE,
} IRValueType;

typedef struct {
    IRValueType type;
    union {
        struct {
            int val;
        } constant;
        struct {
            const char* identifier;
        } var;
    } inner;
} IRValue;

// INSTRUCTION
typedef enum {
    IR_INSTRUCTION_RETURN,
    IR_INSTRUCTION_UNARY,
    IR_INSTRUCTION_BINARY,
} IRInstructionType;

typedef struct {
    IRInstructionType type;
    union {
        struct {
            IRValue val;
        } ret;
        struct {
            UnaryOperator op;
            IRValue src;
            IRValue dst;
        } unary;
        struct {
            BinaryOperator op;
            IRValue left;
            IRValue right;
            IRValue dst;
        } binary;
    } inner;
} IRInstruction;

typedef struct {
    IRInstruction* inner;
    size_t size;
} IRInstructions;

// PROGRAM
typedef struct {
    const char* name;
    IRInstructions body;
} IRFunctionDefinition;

typedef struct {
    IRFunctionDefinition func;
} IRProgram;

void print_IR_program(IRProgram* program);

#endif
#ifndef IR_H
#define IR_H

#include <stddef.h>
#include <stdlib.h>

#include "../parser/ast.h"

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
IRProgram IR_transform_ast(CProgram ast);

#endif
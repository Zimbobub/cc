#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    OPERATOR_ADD,
    OPERATOR_SUB,
    OPERATOR_MUL,
    OPERATOR_DIV,
} Operator;


// EXPRESSIONS
typedef struct {
    int left;
    int right;
    Operator op;
} BinaryExpression;

typedef struct {
    int val;
    Operator op;
} UnaryExpression;

typedef struct {
    int val;
} ConstantExpression;

typedef enum {
    EXPRESSION_CONST,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY
} ExpressionType;

typedef struct {
    ExpressionType type;
    union {
        ConstantExpression constant;
        UnaryExpression unary;
        BinaryExpression binary;
    } expr;
} Expression;


// STATEMENTS
typedef enum {
    STATEMENT_RETURN
} StatementType;

typedef struct {
    StatementType type;
    union {
        Expression ret;
    } statement;
} Statement;


// PROGRAM
typedef struct {
    char* name;
    Statement body;
} FunctionDefinition;

typedef struct {
    FunctionDefinition function_definition;
} Program;



#endif
#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdio.h>


// OPERATORS
// repr char for easy printing
typedef enum {
    OPERATOR_ADD = '+',
    OPERATOR_SUB = '-',
    OPERATOR_MUL = '*',
    OPERATOR_DIV = '/',
} CBinaryOperator;

typedef enum {
    OPERATOR_BITWISE_COMPLEMENT = '~',
    OPERATOR_NEGATE = '-'
} UnaryOperator;



// EXPRESSIONS
typedef enum {
    EXPRESSION_CONST,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY
} CExpressionType;

// forward declaration (for pointers)
typedef struct CExpression CExpression;

typedef struct {
    CExpression* left;
    CExpression* right;
    CBinaryOperator op;
} CBinaryExpression;

typedef struct {
    CExpression* val;
    UnaryOperator op;
} CUnaryExpression;

typedef struct {
    int val;
} CConstantExpression;

typedef struct CExpression {
    CExpressionType type;
    union {
        CConstantExpression constant;
        CUnaryExpression unary;
        CBinaryExpression binary;
    } expr;
} CExpression;



// STATEMENTS
typedef enum {
    STATEMENT_RETURN
} CStatementType;

typedef struct {
    CStatementType type;
    union {
        CExpression ret;
    } statement;
} CStatement;



// PROGRAM
typedef struct {
    char* name;
    CStatement body;
} CFunctionDefinition;

typedef struct {
    CFunctionDefinition function_definition;
} CProgram;




void print_c_expr(CExpression* expr, int depth);
void print_c_statement(CStatement* stmnt, int depth);
void print_c_function_definition(CFunctionDefinition* func, int depth);
void print_c_program(CProgram* program);


#endif
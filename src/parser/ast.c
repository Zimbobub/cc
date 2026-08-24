#include "ast.h"

void print_c_expr(CExpression* expr, int depth) {
    if (expr->type == EXPRESSION_CONST) {
        printf("%*c%d\n", depth, ' ', expr->expr.constant.val);
    } else if (expr->type == EXPRESSION_UNARY) {
        printf("%*c%c\n", depth, ' ', expr->expr.unary.op); // operators repr char
        print_c_expr(expr->expr.unary.val, depth+4);
    } else if (expr->type == EXPRESSION_BINARY) {
        print_c_expr(expr->expr.binary.left, depth+4);
        printf("%*c%c\n", depth, ' ', expr->expr.binary.op);
        print_c_expr(expr->expr.binary.right, depth+4);
    }
}

void print_c_statement(CStatement* stmnt, int depth) {
    printf("%*cStatement (return) {\n", depth, ' ');
    print_c_expr(&stmnt->statement.ret, depth+4);
    printf("%*c}\n", depth, ' ');
}

void print_c_function_definition(CFunctionDefinition* func, int depth) {
    printf("%*cFunctionDefinition (%s) {\n", depth, ' ', func->name);
    print_c_statement(&func->body, depth+4);
    printf("%*c}\n", depth, ' ');
}

void print_c_program(CProgram* program) {
    printf("CProgram {\n");
    print_c_function_definition(&program->function_definition, 4);
    printf("}\n");
}

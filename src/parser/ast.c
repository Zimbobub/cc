#include "ast.h"

void print_c_expr(CExpression* expr, int depth) {
    printf("%*cExpression {\n", depth, ' ');
    printf("%*c%d\n", depth+2, ' ', expr->expr.constant.val);
    printf("%*c}\n", depth, ' ');
}

void print_c_statement(CStatement* stmnt, int depth) {
    printf("%*cStatement (return) {\n", depth, ' ');
    print_c_expr(&stmnt->statement.ret, depth+2);
    printf("%*c}\n", depth, ' ');
}

void print_c_function_definition(CFunctionDefinition* func, int depth) {
    printf("%*cFunctionDefinition (%s) {\n", depth, ' ', func->name);
    print_c_statement(&func->body, depth+2);
    printf("%*c}\n", depth, ' ');
}

void print_c_program(CProgram* program) {
    printf("CProgram {\n");
    print_c_function_definition(&program->function_definition, 2);
    printf("}\n");
}

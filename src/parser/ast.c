#include "ast.h"

void print_expr(Expression* expr, size_t depth) {
    printf("%*cExpression {\n", depth, ' ');
    printf("%*c%d\n", depth+2, ' ', expr->expr.constant.val);
    printf("%*c}\n", depth, ' ');
}

void print_statement(Statement* stmnt, size_t depth) {
    printf("%*cStatement (return) {\n", depth, ' ');
    print_expr(&stmnt->statement.ret, depth+2);
    printf("%*c}\n", depth, ' ');
}

void print_function_definition(FunctionDefinition* func, size_t depth) {
    printf("%*cFunctionDefinition (%s) {\n", depth, ' ', func->name);
    print_statement(&func->body, depth+2);
    printf("%*c}\n", depth, ' ');
}

void print_program(Program* program) {
    printf("Program {\n");
    print_function_definition(&program->function_definition, 2);
    printf("}\n");
}

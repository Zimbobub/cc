#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token.h"
#include "ast.h"

typedef struct {
    TokenBuf tokens;
    Program* ast;
} Parser;

Parser* parser_init(TokenBuf tokens);
Program* parser_destruct(Parser* this);
bool parser_run(Parser* this);

#endif
#include "token.h"

void print_token(Token *token) {
    static const char *token_types[] = {
        "Unknown",

        "Keyword",
        "Identifier",
        "Number",
        "StringLiteral",
    
        "Semicolon",
        "Period",
        "Comma",
    
        "LParen",
        "RParen",
        "LBracket",
        "RBracket",
        "LBrace",
        "RBrace"
    };
    
    printf("%3ld:%-3ld type: %2d %10s '%s'\n", token->line_num, token->col_num, token->type, token_types[token->type], token->src);
}

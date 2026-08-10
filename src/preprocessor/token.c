#include "token.h"

void print_token(Token *token) {
    static const char *token_types[] = {
        "Unknown",

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
    
    printf("%ld:%ld type: %d %15s '%s'\n", token->line_num, token->col_num, token->type, token_types[token->type], token->src);
}

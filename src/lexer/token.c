#include "token.h"

char* get_token_name(TokenType type) {
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
    return token_types[type];
}

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

void tokenbuf_destruct(TokenBuf* this) {
    for (int i = 0; i < this->n_tokens; i++) {
        free(this->tokens[i].src);
    }
    free(this->tokens);
}
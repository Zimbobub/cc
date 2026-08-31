#include "token.h"

const char* get_token_name(TokenType type) {
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
        "RBrace",

        "Tilde",
        "Plus",
        "Minus",
        "Asterisk",
        "ForwardSlash",
        "Percent",
    
        "Increment",
        "Decrement"
    };
    return token_types[type];
}

void print_token(Token *token) {
    size_t width = strlen(token->file_name) + 8;
    size_t location_width = snprintf(NULL, 0, "%s:%ld:%ld", token->file_name, token->line_num, token->col_num);
    int padding = width > location_width ? width - location_width : 0;
    printf("%s:%ld:%ld %*c type: %2d %10s '%s'\n", token->file_name, token->line_num, token->col_num, padding, ' ', token->type, get_token_name(token->type), token->src);
}

void print_tokens(TokenBuf *tokens) {
    for (size_t i = 0; i < tokens->n_tokens; ++i) {
       print_token(&tokens->tokens[i]);
    }
}

void tokenbuf_destruct(TokenBuf* this) {
    for (size_t i = 0; i < this->n_tokens; i++) {
        free(this->tokens[i].src);
    }
    free(this->tokens);
}
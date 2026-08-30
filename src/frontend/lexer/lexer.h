#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "token.h"

typedef struct {
    // position
    const char *file_name;
    size_t line;
    size_t col;
    FILE *fp;

    // input
    size_t input_buf_max_size;
    size_t input_buf_size;
    char *input_buf;

    // token buf
    size_t max_tokens;
    size_t n_tokens;
    Token *tokens;

    // error
    // bool err;
    // char *err_msg; // undefined unless err=true
    // size_t err_len;
} Lexer;

// Lexer* lexer_init(const char* file_name);
// free everything except the tokens
// TokenBuf lexer_destruct(Lexer *this);
// return true if err
TokenBuf lexer_run(const char* preprocessed_file_name);

// requires Lexer type to exist
#include "../compile_error/compile_error.h"

#endif

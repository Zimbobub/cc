#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

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
    char *file_name;
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
} Preprocessor;

Preprocessor* preprocessor_init(const char* file_name);
void preprocessor_destruct(Preprocessor *this);
void preprocessor_run(Preprocessor *this);

#endif

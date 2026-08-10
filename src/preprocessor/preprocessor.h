#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdlib.h>
#include <stddef.h>

#include "token.h"

typedef struct {
    size_t src_buf_size;
    char *src_buf;

    size_t token_buf_size;
    Token *token_buf;
} Preprocessor;

#endif
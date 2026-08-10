#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

#include "token.h"

void run_preprocessor(const char* file_name, Token* tokens, size_t* num_tokens);

#endif
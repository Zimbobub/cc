#ifndef COMPILE_ERROR_H
#define COMPILE_ERROR_H

#include "stdio.h"
#include "stdarg.h"

#include "../lexer/lexer.h"
#include "../lexer/token.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void lexer_error(const Lexer* this, const char* fmt, ...);

#endif
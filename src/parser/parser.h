#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/token.h"
#include "ast.h"

Program parse_program(TokenBuf tokens);

#endif
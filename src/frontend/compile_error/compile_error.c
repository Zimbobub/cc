#include "compile_error.h"

void lexer_error(const Lexer* this, const char* fmt, ...) {
    // header
    fprintf(stderr, "Lexer Error at %s:%ld:%ld\n", this->file_name, this->line, this->col);
    // err msg
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    if (this->n_tokens > 0) {
        // print err location
        // go back to first token of the line
        size_t first_token_on_line = this->n_tokens-1;
        while (first_token_on_line >= 0 && this->tokens[first_token_on_line].line_num == this->line) {
            first_token_on_line--;
        }
        first_token_on_line++; // go forward 1 token

        for (size_t i = first_token_on_line; i < this->n_tokens; i++) {
            fprintf(stderr, "%s ", this->tokens[i].src);
        }
        fprintf(stderr, "%s%s%s <-- here\n", ANSI_COLOR_RED, this->input_buf, ANSI_COLOR_RESET);
    }

    exit(EXIT_FAILURE);
}

void parser_error(const TokenBuf* this, size_t* token_index, const char* fmt, ...) {
    // hehader
    fprintf(stderr, "Syntax Error %s:%ld:%ld\n", this->tokens[*token_index].file_name, this->tokens[*token_index].line_num, this->tokens[*token_index].col_num);
    // err msg
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    if (this->n_tokens > 0) {
        // print err location
        // go back to first token of the line
        size_t i = *token_index;
        while (i >= 0 && this->tokens[i].line_num == this->tokens[*token_index].line_num) {
            i--;
        }
        i++; // go forward 1 token

        // go forward to end of line, print err token in red
        while (i < this->n_tokens && this->tokens[i].line_num == this->tokens[*token_index].line_num) {
            if (i == *token_index) fprintf(stderr, "%s%s%s ", ANSI_COLOR_RED, this->tokens[i].src, ANSI_COLOR_RESET);
            else fprintf(stderr, "%s ", this->tokens[i].src);
            i++;
        }
        fprintf(stderr, "<-- here\n");
    }

    exit(EXIT_FAILURE);
}
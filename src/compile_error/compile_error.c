#include "compile_error.h"

void lexer_error(const Lexer* this, const char* fmt, ...) {
    // hehader
    fprintf(stderr, "Lexer Error at %s:%ld:%ld\n", this->file_name, this->line, this->col);
    // err msg
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    if (this->n_tokens > 0) {
        // print err location
        // go back to first token of the line
        size_t first_token_on_line = this->n_tokens-1;
        while (first_token_on_line >= 0 && this->tokens[first_token_on_line].line_num == this->line) {
            first_token_on_line--;
        }

        for (size_t i = first_token_on_line; i < this->n_tokens; i++) {
            fprintf(stderr, "%s ", this->tokens[i].src);
        }
        fprintf(stderr, "%s%s%s <-- here\n", ANSI_COLOR_RED, this->input_buf, ANSI_COLOR_RESET);
    }

    exit(EXIT_FAILURE);
}
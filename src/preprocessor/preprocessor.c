#include "preprocessor.h"


void run_preprocessor(const char *file_name, Token *tokens, size_t *num_tokens) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("Unable to open file.");
        exit(EXIT_FAILURE);
    }

    size_t current_token_str_size = 100;
    char* current_token = malloc(sizeof(char) * current_token_str_size);

    size_t token_buf_size = 1000;
    tokens = malloc(sizeof(Token) * token_buf_size);

    if (current_token == nullptr || tokens == nullptr) {
        printf("Failed to allocate preprocessor.");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    *num_tokens = 0;

    // tokenise loop
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) {
            // add token to
        }
    }

    fclose(fp);
}
#include "preprocessor.h"

TokenType get_single_char_token_type(char c);
TokenType get_double_char_token_type(char c1, char c2);

void run_preprocessor(const char *file_name, Token *tokens, size_t *num_tokens) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("Unable to open file.");
        exit(EXIT_FAILURE);
    }

    size_t current_token_max_size = 100;
    size_t current_token_size = 0;
    char* current_token = malloc(sizeof(char) * current_token_max_size);

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
    size_t line = 1;
    size_t col = 0;

    #define IS_IDENTIFIER 0x1
    #define IS_NUMBER 0x2
    #define IS_SINGLE_CHAR 0x4
    #define IS_DOUBLE_CHAR 0x8
    uint8_t prev_token_type_bitset = 0b00001111;
    uint8_t token_type_bitset = 0b00001111;

    while ((c = fgetc(fp)) != EOF) {
        // skip over whitespace before anything else
        if (c == '\n') {
            line++;
            col = 0;
            continue;
        } else if (isspace(c)) continue;

        col++;



        // grow current_token each loop until only 1 token type bool remains
        if (current_token_size >= current_token_max_size-1) {
            current_token_max_size *= 2;
            current_token = realloc(current_token, current_token_max_size);
            if (current_token == nullptr) {
                printf("Failed to reallocate.");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
        }
        current_token[current_token_size] = c;
        current_token_size++;



        // keyword / identifier
        if (current_token_size == 1 && isdigit(c)) token_type_bitset &= ~IS_IDENTIFIER;
        if (!isalnum(c) || c != '_') token_type_bitset &= ~IS_IDENTIFIER;

        // number literal
        if (!isdigit(c)) token_type_bitset &= ~IS_NUMBER;

        // single / double char
        if (current_token_size > 1) token_type_bitset &= ~IS_SINGLE_CHAR;
        if (current_token_size > 2) token_type_bitset &= ~IS_DOUBLE_CHAR;



        // once token_type_bitset gets to 0, see which type in the previous loop had its bit set last
        if (token_type_bitset == 0) {
            Token token = {0};
            strncpy(token.src, current_token, current_token_size-1); // dont copy last char
            token.line_num = line;
            token.col_num = col;

            if (prev_token_type_bitset == IS_IDENTIFIER) token.type = Identifier;
            else if (prev_token_type_bitset == IS_NUMBER) token.type = Number;
            else if (prev_token_type_bitset == IS_SINGLE_CHAR) token.type = get_single_char_token_type(current_token[0]);
            else if (prev_token_type_bitset == IS_DOUBLE_CHAR) token.type = get_double_char_token_type(current_token[0], current_token[1]);
            else {
                printf("Unknown token type %x at %d:%d\n", prev_token_type_bitset, line, col);
                fclose(fp);
                exit(EXIT_FAILURE);
            }

            tokens[*num_tokens] = token;
            *num_tokens++;

            // reset current_token
            memset(current_token, 0, current_token_size);
            current_token_size = 1;
            current_token[0] = c;

            // reset bitsets
            token_type_bitset = 0b00001111;
            prev_token_type_bitset = 0b00001111;
        }
    }

    fclose(fp);
}


TokenType get_single_char_token_type(char c) {
    if (c == ';') return Semicolon;
    else if (c == '.') return Period;
    else if (c == ',') return Comma;
    else if (c == '(') return LParen;
    else if (c == ')') return RParen;
    else if (c == '[') return LBracket;
    else if (c == ']') return RBracket;
    else if (c == '{') return LBrace;
    else if (c == '}') return RBrace;
}

TokenType get_double_char_token_type(char c1, char c2) {
    // TODO
}
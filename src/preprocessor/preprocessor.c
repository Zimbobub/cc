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

    if (current_token == NULL || tokens == NULL) {
        printf("Failed to allocate preprocessor.");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    memset(current_token, 0, current_token_max_size);
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
        if (c == '\n') {
            line++;
            col = 0;
        }
        col++;

        if (current_token_size == 0 && isspace(c)) continue;


        // keyword / identifier
        if (current_token_size == 0 && isdigit(c)) token_type_bitset &= ~IS_IDENTIFIER;
        if (!isalnum(c) && c != '_') token_type_bitset &= ~IS_IDENTIFIER;

        // number literal
        if (!isdigit(c)) token_type_bitset &= ~IS_NUMBER;

        // single / double char
        if (current_token_size > 1 || 
            (current_token_size == 1 && get_single_char_token_type(current_token[0]) == Unknown) ||
            !ispunct(current_token[0])
        ) {
            token_type_bitset &= ~IS_SINGLE_CHAR;
        }
        if (current_token_size > 2 || 
            (current_token_size == 2 && get_double_char_token_type(current_token[0], current_token[1]) == Unknown) ||
            (current_token_size == 1 && get_single_char_token_type(current_token[0]) != Unknown) ||
            !ispunct(current_token[0]) || !ispunct(current_token[1])
        ) {
            token_type_bitset &= ~IS_DOUBLE_CHAR;
        }

        printf("%ld:%ld %c (0x%d, 0x%d) '%s' %ld\n", line, col, c, prev_token_type_bitset, token_type_bitset, current_token, current_token_size);

        // once token_type_bitset gets to 0, see which type in the previous loop had its bit set last
        if (token_type_bitset == 0 || 
            token_type_bitset == IS_SINGLE_CHAR ||
            token_type_bitset == IS_DOUBLE_CHAR
        ) {
            Token token = {0};
            token.line_num = line;
            token.col_num = col;

            size_t chars_consumed = 0;

            if (prev_token_type_bitset & IS_IDENTIFIER) {
                token.type = Identifier;
                chars_consumed = current_token_size;
            }
            else if (prev_token_type_bitset & IS_NUMBER) {
                token.type = Number;
                chars_consumed = current_token_size;
            }
            else if (prev_token_type_bitset & IS_SINGLE_CHAR) {
                token.type = get_single_char_token_type(current_token[0]);
                chars_consumed = 1;
            }
            else if (prev_token_type_bitset & IS_DOUBLE_CHAR) {
                token.type = get_double_char_token_type(current_token[0], current_token[1]);
                chars_consumed = 2;
            }
            else {
                printf("Unknown token type %x at %ld:%ld, '%s'\n", prev_token_type_bitset, line, col, current_token);
                fclose(fp);
                exit(EXIT_FAILURE);
            }

            // copy src
            token.src = malloc(chars_consumed+1);
            if (token.src == NULL) {
                printf("Failed to allocate token.");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
            strncpy(token.src, current_token, chars_consumed);

            print_token(&token);

            tokens[*num_tokens] = token;
            (*num_tokens)++;


            // reset current_token
            // trim whitespace at the start
            size_t spaces = 0;
            while (isspace(current_token[chars_consumed + spaces])) spaces++;
            // shift all characters to the left for each char consumed
            strcpy(current_token, current_token + chars_consumed + spaces);
            // set all characters after the \0 to \0
            memset(current_token + (current_token_size - chars_consumed), 0, current_token_max_size - chars_consumed);

            current_token_size = strlen(current_token);


            // reset bitsets
            token_type_bitset = 0b00001111;
            prev_token_type_bitset = 0b00001111;
        } else {
            prev_token_type_bitset = token_type_bitset;
        }
        // add char to buffer
        if (!isspace(c)) {
            // grow current_token each loop until only 1 token type bool remains
            if (current_token_size >= current_token_max_size-1) {
                current_token_max_size *= 2;
                current_token = realloc(current_token, current_token_max_size);
                if (current_token == NULL) {
                    printf("Failed to reallocate.");
                    fclose(fp);
                    exit(EXIT_FAILURE);
                }
            }

            current_token[current_token_size] = c;
            current_token_size++;
        }
    }

    fclose(fp);

    if (current_token_size > 0) {
        printf("Error: Finished parsing with '%s' remaining at EOF (make sure to add a newline at the end of the file)\n", current_token);
        exit(EXIT_FAILURE);
    }
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
    else return Unknown;
}

TokenType get_double_char_token_type(char c1, char c2) {
    return Unknown;
}
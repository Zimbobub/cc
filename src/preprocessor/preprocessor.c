#include "preprocessor.h"

TokenType get_single_char_token_type(char c);
TokenType get_double_char_token_type(char c1, char c2);

// void old_run_preprocessor(const char *file_name, Token *tokens, size_t *num_tokens) {
//     FILE *fp = fopen(file_name, "r");

//     if (fp == NULL) {
//         printf("Unable to open file.");
//         exit(EXIT_FAILURE);
//     }

//     size_t current_token_max_size = 100;
//     size_t current_token_size = 0;
//     char* current_token = malloc(sizeof(char) * current_token_max_size);

//     size_t token_buf_size = 1000;
//     tokens = malloc(sizeof(Token) * token_buf_size);

//     if (current_token == NULL || tokens == NULL) {
//         printf("Failed to allocate preprocessor.");
//         fclose(fp);
//         exit(EXIT_FAILURE);
//     }

//     memset(current_token, 0, current_token_max_size);
//     *num_tokens = 0;

//     // tokenise loop
//     char c;
//     size_t line = 1;
//     size_t col = 0;

//     #define IS_IDENTIFIER 0x1
//     #define IS_NUMBER 0x2
//     #define IS_SINGLE_CHAR 0x4
//     #define IS_DOUBLE_CHAR 0x8
//     uint8_t prev_token_type_bitset = 0b00001111;
//     uint8_t token_type_bitset = 0b00001111;

//     while ((c = fgetc(fp)) != EOF) {
//         if (c == '\n') {
//             line++;
//             col = 0;
//         }
//         col++;

//         if (current_token_size == 0 && isspace(c)) continue;


//         // keyword / identifier
//         if (current_token_size == 0 && isdigit(c)) token_type_bitset &= ~IS_IDENTIFIER;
//         if (!isalnum(c) && c != '_') token_type_bitset &= ~IS_IDENTIFIER;

//         // number literal
//         if (!isdigit(c)) token_type_bitset &= ~IS_NUMBER;

//         // single / double char
//         if (current_token_size > 1 || 
//             (current_token_size == 1 && get_single_char_token_type(current_token[0]) == Unknown) ||
//             !ispunct(current_token[0])
//         ) {
//             token_type_bitset &= ~IS_SINGLE_CHAR;
//         }
//         if (current_token_size > 2 || 
//             (current_token_size == 2 && get_double_char_token_type(current_token[0], current_token[1]) == Unknown) ||
//             (current_token_size == 1 && get_single_char_token_type(current_token[0]) != Unknown) ||
//             !ispunct(current_token[0]) || !ispunct(current_token[1])
//         ) {
//             token_type_bitset &= ~IS_DOUBLE_CHAR;
//         }

//         printf("%ld:%ld %c (0x%d, 0x%d) '%s' %ld\n", line, col, c, prev_token_type_bitset, token_type_bitset, current_token, current_token_size);

//         // once token_type_bitset gets to 0, see which type in the previous loop had its bit set last
//         if (token_type_bitset == 0 || 
//             token_type_bitset == IS_SINGLE_CHAR ||
//             token_type_bitset == IS_DOUBLE_CHAR
//         ) {
//             Token token = {0};
//             token.line_num = line;
//             token.col_num = col;

//             size_t chars_consumed = 0;

//             if (prev_token_type_bitset & IS_IDENTIFIER) {
//                 token.type = Identifier;
//                 chars_consumed = current_token_size;
//             }
//             else if (prev_token_type_bitset & IS_NUMBER) {
//                 token.type = Number;
//                 chars_consumed = current_token_size;
//             }
//             else if (prev_token_type_bitset & IS_SINGLE_CHAR) {
//                 token.type = get_single_char_token_type(current_token[0]);
//                 chars_consumed = 1;
//             }
//             else if (prev_token_type_bitset & IS_DOUBLE_CHAR) {
//                 token.type = get_double_char_token_type(current_token[0], current_token[1]);
//                 chars_consumed = 2;
//             }
//             else {
//                 printf("Unknown token type %x at %ld:%ld, '%s'\n", prev_token_type_bitset, line, col, current_token);
//                 fclose(fp);
//                 exit(EXIT_FAILURE);
//             }

//             // copy src
//             token.src = malloc(chars_consumed+1);
//             if (token.src == NULL) {
//                 printf("Failed to allocate token.");
//                 fclose(fp);
//                 exit(EXIT_FAILURE);
//             }
//             strncpy(token.src, current_token, chars_consumed);

//             print_token(&token);

//             tokens[*num_tokens] = token;
//             (*num_tokens)++;


//             // reset current_token
//             // trim whitespace at the start
//             size_t spaces = 0;
//             while (isspace(current_token[chars_consumed + spaces])) spaces++;
//             // shift all characters to the left for each char consumed
//             strcpy(current_token, current_token + chars_consumed + spaces);
//             // set all characters after the \0 to \0
//             memset(current_token + (current_token_size - chars_consumed), 0, current_token_max_size - chars_consumed);

//             current_token_size = strlen(current_token);


//             // reset bitsets
//             token_type_bitset = 0b00001111;
//             prev_token_type_bitset = 0b00001111;
//         } else {
//             prev_token_type_bitset = token_type_bitset;
//         }
//         // add char to buffer
//         if (!isspace(c)) {
//             // grow current_token each loop until only 1 token type bool remains
//             if (current_token_size >= current_token_max_size-1) {
//                 current_token_max_size *= 2;
//                 current_token = realloc(current_token, current_token_max_size);
//                 if (current_token == NULL) {
//                     printf("Failed to reallocate.");
//                     fclose(fp);
//                     exit(EXIT_FAILURE);
//                 }
//             }

//             current_token[current_token_size] = c;
//             current_token_size++;
//         }
//     }

//     fclose(fp);

//     if (current_token_size > 0) {
//         printf("Error: Finished parsing with '%s' remaining at EOF (make sure to add a newline at the end of the file)\n", current_token);
//         exit(EXIT_FAILURE);
//     }
// }


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

bool is_operator(char c) {
    return (strchr("+-*/%<>=", c) != NULL);
}

bool is_delimiter(char c) {
    return (isspace(c) || is_operator(c) || strchr(",.;()[]{}", c) != NULL);
}

bool is_identifier(const char* str, size_t len) {
    if (len == 0) return false;
    for (int i = 0; i < len; i++) {
        if (is_delimiter(str[i])) return false;
    }
    return (!isdigit(str[0]));
}

bool is_keyword(const char* str, size_t len) {
    const char* keywords[] = {
        "auto",     "break",    "case",     "char",
        "const",    "continue", "default",  "do",
        "double",   "else",     "enum",     "extern",
        "float",    "for",      "goto",     "if",
        "int",      "long",     "register", "return",
        "short",    "signed",   "sizeof",   "static",
        "struct",   "switch",   "typedef",  "union",
        "unsigned", "void",     "volatile", "while" 
    };

    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return true;
    }
    return false;
}

bool is_int(const char *str, size_t len) {
    if (len == 0) return false;
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

void add_token(Preprocessor *this,  TokenType type) {
    // make token
    Token token = {0};
    token.line_num = this->line;
    token.col_num = this->col;
    token.type = type;
    token.src = malloc(this->input_buf_size+1);
    if (token.src == NULL) {
        printf("Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(token.src, this->input_buf);
    clear_buffer(this);

    // add to tokens
    if (this->n_tokens>= this->max_tokens-1) {
        this->max_tokens *= 2;
        this->tokens = realloc(this->tokens, this->max_tokens);
        if (this->tokens == NULL) {
            printf("Realloc failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    this->tokens[this->n_tokens] = token;
    this->n_tokens++;
}

void add_char(Preprocessor *this, char c) {
    if (this->input_buf_size >= this->input_buf_max_size-1) {
        this->input_buf_max_size *= 2;
        this->input_buf = realloc(this->input_buf, this->input_buf_max_size);
        if (this->input_buf == NULL) {
            printf("Realloc failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    this->input_buf[this->input_buf_size] = c;
    this->input_buf_size++;
}

void consume_chars(Preprocessor *this, size_t num_chars) {
    char *temp_str = malloc(this->input_buf_max_size);
    if (temp_str == NULL) {
        printf("Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // strcpy for overlapping ptrs is undefined behaviour, so use an intermediary
    strcpy(temp_str, this->input_buf + num_chars);
    strcpy(this->input_buf, temp_str);
}

void clear_buffer(Preprocessor *this) {
    memset(this->input_buf, 0, this->input_buf_max_size);
    this->input_buf_size = 0;
}

Preprocessor* preprocessor_init(const char* file_name) {
    Preprocessor *this = malloc(sizeof(Preprocessor));
    if (this == NULL) {
        printf("Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    this->file_name = file_name;
    this->line = 1;
    this->col = 1;

    this->fp = fopen(file_name, "r");
    if (this->fp == NULL) {
        printf("Unable to open file.");
        exit(EXIT_FAILURE);
    }

    this->input_buf_max_size = 100;
    this->input_buf_size = 0;
    this->input_buf = malloc(this->input_buf_max_size);
    this->input_buf[0] = '\0';

    this->max_tokens = 100;
    this->n_tokens = 0;
    this->tokens = malloc(this->max_tokens);

    if (this->input_buf == NULL || this->tokens == NULL) {
        printf("Malloc failed");
        exit(EXIT_FAILURE);
    }

    return this;
}


void preprocessor_run(Preprocessor *this) {
    // left = input_buf[0]
    // right = input_buf[len-1]

    int c;
    while (c != EOF) {
        // if within a token, add next char to buf
        if (!is_delimiter(this->input_buf[this->input_buf_size-1])) {
            c = fgetc(this->fp);
            add_char(this, c);
        } 

        // single char token
        if (is_delimiter(this->input_buf[this->input_buf_size-1]) && this->input_buf_size == 1) {
            // if (is_operator(this->input_buf[this->input_buf_size-1])) {
            //     printf("operator %c\n", this->input_buf[this->input_buf_size-1]);
                
            // }
            printf("token '%c'\n", this->input_buf[this->input_buf_size-1]);
            add_token(this, get_single_char_token_type(this->input_buf[0]));

            // consume 1 char, and add the next
            // consume_chars(this, 1);
            c = fgetc(this->fp);
            add_char(this, c);
        } else if (is_delimiter(this->input_buf[this->input_buf_size-1]) && this->input_buf_size != 0/* EOF || (this->input_buf_size == 0)*/) {
            // substr = input_buf

            if (is_keyword(this->input_buf, this->input_buf_size)) {                
                printf("keyword '%s'\n", this->input_buf);
                add_token(this, Keyword);
            } else if (is_int(this->input_buf, this->input_buf_size)) {
                printf("number '%s'\n", this->input_buf);
                add_token(this, Number);
            } else if (is_identifier(this->input_buf, this->input_buf_size)) {
                printf("identifier '%s'\n", this->input_buf);
                add_token(this, Identifier);
            } else {
                printf("Unknown token: '%s'\n", this->input_buf);
            }

            clear_buffer(this);

            c = fgetc(this->fp);
            add_char(this, c);
        }
    }
}

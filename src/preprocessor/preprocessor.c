#include "preprocessor.h"

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
        if (!isalnum(str[i]) && str[i] != '_') return false;
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

//
// PREPROCESSOR HELPER METHODS
//

void clear_buffer(Preprocessor *this) {
    memset(this->input_buf, 0, this->input_buf_max_size);
    this->input_buf_size = 0;
}

void consume_token(Preprocessor *this,  TokenType type) {
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
        c = fgetc(this->fp);
        if (c == EOF) break;

        // if within a token, add next char to buf
        if (!is_delimiter(this->input_buf[this->input_buf_size-1])) {
            add_char(this, c);
        } 

        // single char token
        if (is_delimiter(this->input_buf[this->input_buf_size-1]) && this->input_buf_size == 1) {
            // try make 2 char special
            c = fgetc(this->fp);
            TokenType double_token = get_double_char_token_type(this->input_buf[0], c);
            if (double_token != Unknown) {
                // 2 char
                printf("token '%c'\n", this->input_buf[0]);
                add_char(this, c);
                consume_token(this, double_token);
            } else {
                // 1 char
                TokenType single_token = get_single_char_token_type(this->input_buf[0]);
                if (single_token == Unknown) {
                    printf("Unknown single char token type: '%c'\n", this->input_buf[0]);
                    fclose(this->fp);
                    exit(EXIT_FAILURE);
                }
                printf("token '%c'\n", this->input_buf[0]);
                consume_token(this, single_token);
                add_char(this, c);
            }

            // if (is_operator(this->input_buf[this->input_buf_size-1])) {
            //     printf("operator %c\n", this->input_buf[this->input_buf_size-1]);
                
            // }
            printf("token '%c'\n", this->input_buf[this->input_buf_size-1]);
            consume_token(this, get_single_char_token_type(this->input_buf[0]));

            // consume 1 char, and add the next
            // consume_chars(this, 1);
            clear_buffer(this);
            add_char(this, c);
        } else if (is_delimiter(this->input_buf[this->input_buf_size-1]) && this->input_buf_size != 0/* EOF || (this->input_buf_size == 0)*/) {
            // substr = input_buf

            if (is_keyword(this->input_buf, this->input_buf_size)) {                
                printf("keyword '%s'\n", this->input_buf);
                consume_token(this, Keyword);
            } else if (is_int(this->input_buf, this->input_buf_size)) {
                printf("number '%s'\n", this->input_buf);
                consume_token(this, Number);
            } else if (is_identifier(this->input_buf, this->input_buf_size)) {
                printf("identifier '%s'\n", this->input_buf);
                consume_token(this, Identifier);
            } else {
                printf("Unknown token: '%s'\n", this->input_buf);
            }

            // clear_buffer(this);
            add_char(this, c);
        }
    }

    printf("Finished preprocessing with remaining '%s'\n", this->input_buf);
}

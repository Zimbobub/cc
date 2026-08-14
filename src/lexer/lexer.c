#include "lexer.h"

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
    if (c1 == '+' && c2 == '+') return Increment;
    else if (c1 == '-' && c2 == '-') return Decrement;
    else return Unknown;
}

bool is_operator(char c) {
    return (strchr("+-*/%<>=", c) != NULL);
}

bool is_delimiter(char c) {
    return (isspace(c) || is_operator(c) || (strchr(",.;()[]{}", c) != NULL));
}

bool is_identifier(const char* str, size_t len) {
    if (len == 0) return false;
    for (size_t i = 0; i < len; i++) {
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

    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        size_t keyword_len = strlen(keywords[i]);
        if (len == keyword_len && memcmp(str, keywords[i], len) == 0) return true;
    }
    return false;
}

bool is_int(const char *str, size_t len) {
    if (len == 0) return false;
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

//
// LEXER HELPER METHODS
//

void clear_buffer(Lexer *this) {
    memset(this->input_buf, 0, this->input_buf_max_size);
    this->input_buf_size = 0;
}

void consume_token(Lexer *this,  TokenType type) {
    // make token
    Token token = {0};
    token.file_name = this->file_name; // can just copy the pointer
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
    if (this->n_tokens >= this->max_tokens-1) {
        this->max_tokens *= 2;
        this->tokens = realloc(this->tokens, sizeof(Token) * this->max_tokens);
        if (this->tokens == NULL) {
            printf("Realloc failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    this->tokens[this->n_tokens] = token;
    this->n_tokens++;
}

void add_char(Lexer *this, char c) {
    // if (isspace(c)) return;

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
    this->input_buf[this->input_buf_size] = '\0';
}

void consume_chars(Lexer *this, size_t num_chars) {
    char *temp_str = malloc(this->input_buf_max_size);
    if (temp_str == NULL) {
        printf("Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // strcpy for overlapping ptrs is undefined behaviour, so use an intermediary
    strcpy(temp_str, this->input_buf + num_chars);
    strcpy(this->input_buf, temp_str);
}

Lexer* lexer_init(const char* file_name) {
    Lexer *this = malloc(sizeof(Lexer));
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
    memset(this->input_buf, 0, this->input_buf_max_size);

    this->max_tokens = 100;
    this->n_tokens = 0;
    this->tokens = malloc(sizeof(Token) * this->max_tokens);

    if (this->input_buf == NULL || this->tokens == NULL) {
        printf("Malloc failed");
        exit(EXIT_FAILURE);
    }

    this->err = false;

    return this;
}


TokenBuf lexer_destruct(Lexer *this) {
    TokenBuf tokens = {
        this->tokens,
        this->n_tokens
    };

    fclose(this->fp);
    free(this->input_buf);

    // for (int i = 0; i < this->n_tokens; i++) {
    //     free(this->tokens[i].src);
    // }
    // free(this->tokens);

    if (this->err) {
        free(this->err_msg);
    }

    free(this);

    return tokens;
}


bool lexer_run(Lexer *this) {
    int c = 0;
    bool c_was_EOF = false;
    while (!c_was_EOF) {
        c = fgetc(this->fp);
        // allow flushing of any text still in the buffer when we reach end of file by preternding EOF is whitespace
        // we then exit at the end of this loop
        if (c == EOF) {
            c_was_EOF = true;
            c = ' ';
        };

        // printf("%s:%ld:%ld '%c' 0x%x len:%ld '%s'\n", this->file_name, this->line, this->col, c, c, this->input_buf_size, this->input_buf);

        this->col++;
        if (c == '\n') {
            this->line++;
            this->col = 1;
        }

        if (isspace(c) && this->input_buf_size == 0) continue;

        if (this->input_buf_size == 1 && is_delimiter(this->input_buf[0])) {
            // try make 1 or 2 char token
            TokenType double_token = get_double_char_token_type(this->input_buf[0], c);
            TokenType single_token = get_single_char_token_type(this->input_buf[0]);
            if (double_token != Unknown) {
                // 2 char
                printf("token '%c%c'\n", this->input_buf[0], c);
                add_char(this, c);
                consume_token(this, double_token);
            } else if (single_token != Unknown) {
                // 1 char
                printf("token '%c'\n", this->input_buf[0]);
                consume_token(this, single_token);
                if (!isspace(c)) add_char(this, c);
            } else {
                this->err_len = asprintf(&this->err_msg, "Unknown single char token type: %s:%ld:%ld '%c' 0x%x\n", this->file_name, this->line, this->col, this->input_buf[0], this->input_buf[0]);
                this->err = true;
                return true;
                // exit(EXIT_FAILURE);
            }

        } else if (this->input_buf_size == 0 || !is_delimiter(c)) {
            // if within a token, add next char to buf
            add_char(this, c);
        } else {
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
                // printf("Unknown token: '%s'\n", this->input_buf);
                this->err_len = asprintf(&this->err_msg, "Unknown token: %s:%ld:%ld '%s'\n", this->file_name, this->line, this->col, this->input_buf);
                this->err = true;
                return true;
                //exit(EXIT_FAILURE);
                // consume_token(this, Unknown);
            }

            if (!isspace(c)) add_char(this, c);
        }
    }

    printf("Finished preprocessing with remaining '%s'\n", this->input_buf);

    return false;
}

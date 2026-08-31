#include "util.h"



const char* operator_to_string(int operator) {
    switch (operator) {
        // unary
        case OPERATOR_BITWISE_COMPLEMENT: return "-";
        case OPERATOR_NEGATE: return "-";

        // binary
        case OPERATOR_ADD: return "+";
        case OPERATOR_SUB: return "-";
        case OPERATOR_MUL: return "*";
        case OPERATOR_DIV: return "/";
        case OPERATOR_MOD: return "%";
    
        case OPERATOR_BITWISE_AND: return "&";
        case OPERATOR_BITWISE_OR: return "|";
        case OPERATOR_BITWISE_XOR: return "^";
        case OPERATOR_LEFT_SHIFT: return "<<";
        case OPERATOR_RIGHT_SHIFT: return ">>";    

        default: return "unknown";
    }
}

Precedence precedence(int operator) {
    switch (operator) {
        // unary
        case OPERATOR_BITWISE_COMPLEMENT: return PRECEDENCE_UNARY;
        case OPERATOR_NEGATE: return PRECEDENCE_UNARY;

        // binary
        case OPERATOR_ADD: return PRECEDENCE_ADD;
        case OPERATOR_SUB: return PRECEDENCE_ADD;
        case OPERATOR_MUL: return PRECEDENCE_MUL;
        case OPERATOR_DIV: return PRECEDENCE_MUL;
        case OPERATOR_MOD: return PRECEDENCE_MUL;
    
        case OPERATOR_BITWISE_AND: return PRECEDENCE_BITWISE_AND;
        case OPERATOR_BITWISE_OR: return PRECEDENCE_BITWISE_OR;
        case OPERATOR_BITWISE_XOR: return PRECEDENCE_BITWISE_XOR;
        case OPERATOR_LEFT_SHIFT: return PRECEDENCE_SHIFT;
        case OPERATOR_RIGHT_SHIFT: return PRECEDENCE_SHIFT;    

        default: return PRECEDENCE_UNKNOWN;
    }
}

Precedence precedence_from_token(TokenType token) {
    switch (token) {
        case Plus:
        case Minus:
            return PRECEDENCE_ADD;

        case Asterisk:
        case ForwardSlash:
        case Percent:
            return PRECEDENCE_MUL;
        
        default:
            return PRECEDENCE_UNKNOWN;
    }
}

UnaryOperator unary_operator_from_token(TokenType token) {
    switch (token) {
        case Tilde: return OPERATOR_BITWISE_COMPLEMENT;
        case Minus: return OPERATOR_NEGATE;

        default: return OPERATOR_UNKNOWN;
    }
}

BinaryOperator binary_operator_from_token(TokenType token) {
    switch (token) {
        case Plus: return OPERATOR_ADD;
        case Minus: return OPERATOR_SUB;
        case Asterisk: return OPERATOR_MUL;
        case ForwardSlash: return OPERATOR_DIV;
        case Percent: return OPERATOR_MOD;

        case Ampersand: return OPERATOR_BITWISE_AND;
        case Pipe: return OPERATOR_BITWISE_OR;
        case Caret: return OPERATOR_BITWISE_XOR;
        case LeftShift: return OPERATOR_LEFT_SHIFT;
        case RightShift: return OPERATOR_RIGHT_SHIFT;
        
        default: return OPERATOR_UNKNOWN;
    }
}


String String_new(size_t cap) {
    if (cap == 0) cap = 4096;

    String string = {
        .ptr=malloc(cap * sizeof(char)),
        .len=0,
        .cap=cap
    };

    if (string.ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    memset(string.ptr, 0, string.cap);

    return string;
}



void StringArena_expand(String* string, size_t required_len) {
    size_t new_cap = (string->cap < required_len) ? (string->cap * 2) : (string->cap);

    char* new_ptr = realloc(string->ptr, string->cap);

    if (new_ptr == NULL) {
        fprintf(stderr, "realloc failed\n");
        exit(EXIT_FAILURE);
    }

    string->ptr = new_ptr;
    string->cap = new_cap;
}


void String_push(String* string, const char* str2) {
    size_t str2_len = strlen(str2);
    size_t required_cap = string->len + strlen(str2) + 1;

    if (required_cap > string->cap) StringArena_expand(string, required_cap);

    strncat(string->ptr + string->len, str2, string->cap);
    string->len += str2_len;
}

void String_push_format(String* string, size_t required_len, const char* fmt, ...) {
    size_t required_cap = string->len + required_len + 1;

    va_list args;
    va_start(args, fmt);
    int chars_written = vsnprintf(string->ptr + string->len, string->cap - string->len, fmt, args);
    va_end(args);

    if (chars_written < 0) {
        fprintf(stderr, "vsnprintf failed\n");
        exit(EXIT_FAILURE);
    }

    string->len += (size_t)chars_written;
}

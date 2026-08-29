#include "util.h"

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

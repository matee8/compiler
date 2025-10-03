#ifndef COMPILER_DS_STRING_BUFFER_H
#define COMPILER_DS_STRING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>

struct string_buffer {
    union {
        struct large_string {
            size_t capacity;
            char* ptr;
        } heap;
        char sso[sizeof(struct large_string) - 1];
    } data;
    size_t len;
    bool on_heap;
};

int string_buffer_init(struct string_buffer* sb);
void string_buffer_destroy(struct string_buffer* sb);
int string_buffer_append(struct string_buffer* sb, const char* str, size_t len);
int string_buffer_append_cstr(struct string_buffer* sb, const char* str);
const char* string_buffer_get_cstr(const struct string_buffer* sb);
char* string_buffer_to_cstr(struct string_buffer* sb);
size_t string_buffer_get_len(const struct string_buffer* sb);

#endif

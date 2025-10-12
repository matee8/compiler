#ifndef COMPILER_DS_SYMBOL_TABLE_H
#define COMPILER_DS_SYMBOL_TABLE_H

#include <stddef.h>

#include "compiler/ds/vector.h"

struct symbol_table_entry {
    const char* key;
    size_t len;
    struct symbol_table_entry* next;
};

struct symbol_table {
    struct vector strings;
    struct symbol_table_entry** buckets;
    size_t capacity;
    size_t count;
};

int symbol_table_init(struct symbol_table* st, size_t initial_capacity);
void symbol_table_destroy(struct symbol_table* st);

int symbol_table_intern(struct symbol_table* st,
                        const char* str,
                        size_t len,
                        size_t* out_index);
const char* symbol_table_get(const struct symbol_table* st, size_t index);
int symbol_table_find(const struct symbol_table* st,
                      const char* str,
                      size_t len,
                      size_t* out_index);

#endif

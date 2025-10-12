#include "compiler/ds/symbol_table.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "compiler/ds/vector.h"

static const size_t SYMBOL_TABLE_DEFAULT_CAPACITY = 16;

int symbol_table_init(struct symbol_table* st, size_t initial_capacity) {
    if (!st) {
        return -EINVAL;
    }

    size_t capacity = (initial_capacity > 0) ? initial_capacity
                                             : SYMBOL_TABLE_DEFAULT_CAPACITY;

    st->count = 0;
    st->capacity = capacity;
    st->buckets = nullptr;

    if (vector_init(&st->strings) != 0) {
        return -ENOMEM;
    }

    st->buckets = (struct symbol_table_entry**)calloc(
        capacity, sizeof(struct symbol_table_entry*));
    if (!st->buckets) {
        vector_destroy(&st->strings);
        return -ENOMEM;
    }

    return 0;
}

void symbol_table_destroy(struct symbol_table* st) {
    if (!st) {
        return;
    }

    for (size_t i = 0; i < vector_len(&st->strings); ++i) {
        free(vector_get(&st->strings, i));
    }

    vector_destroy(&st->strings);

    if (st->buckets) {
        for (size_t i = 0; i < st->capacity; ++i) {
            struct symbol_table_entry* entry = st->buckets[i];
            while (entry) {
                struct symbol_table_entry* next = entry->next;
                free(entry);
                entry = next;
            }
        }
        free((void*)st->buckets);
    }

    st->buckets = nullptr;
    st->capacity = 0;
    st->count = 0;
}

#ifndef COMPILER_DS_VECTOR_H
#define COMPILER_DS_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

struct vector {
    void** data;
    size_t len;
    size_t capacity;
};

int vector_init(struct vector* vec);
int vector_init_with_capacity(struct vector* vec, size_t capacity);
int vector_push(struct vector* vec, void* element);
void* vector_pop(struct vector* vec);
void* vector_get(const struct vector* vec, size_t index);
int vector_set(struct vector* vec, size_t index, void* element);
size_t vector_len(const struct vector* vec);
size_t vector_capacity(const struct vector* vec);
bool vector_is_empty(const struct vector* vec);
void vector_clear(struct vector* vec);
int vector_reserve(struct vector* vec, size_t capacity);
int vector_shrink_to_fit(struct vector* vec);
void vector_destroy(struct vector* vec);

#endif

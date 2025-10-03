#include "compiler/ds/vector.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static const size_t VECTOR_INITIAL_CAPACITY = 8;
static const size_t VECTOR_GROWTH_FACTOR = 2;

static int ensure_capacity(struct vector* vec, size_t required_capacity);

int vector_init(struct vector* vec) {
    if (!vec) {
        return -EINVAL;
    }

    vec->data = nullptr;
    vec->len = 0;
    vec->capacity = 0;

    return 0;
}

int vector_init_with_capacity(struct vector* vec, size_t capacity) {
    if (!vec) {
        return -EINVAL;
    }

    if (capacity == 0) {
        return vector_init(vec);
    }

    vec->data = (void**)malloc(capacity * sizeof(void*));
    if (!vec->data) {
        return -ENOMEM;
    }

    vec->len = 0;
    vec->capacity = capacity;

    return 0;
}

int vector_push(struct vector* vec, void* element) {
    if (!vec) {
        return -EINVAL;
    }

    int result = ensure_capacity(vec, vec->len + 1);
    if (result != 0) {
        return result;
    }

    vec->data[vec->len] = element;
    vec->len++;

    return 0;
}

void* vector_pop(struct vector* vec) {
    if (!vec || vec->len == 0) {
        return nullptr;
    }

    vec->len--;
    return vec->data[vec->len];
}

void* vector_get(const struct vector* vec, size_t index) {
    if (!vec || index >= vec->len) {
        return nullptr;
    }

    return vec->data[index];
}

int vector_set(struct vector* vec, size_t index, void* element) {
    if (!vec || index >= vec->len) {
        return -EINVAL;
    }

    vec->data[index] = element;
    return 0;
}

size_t vector_len(const struct vector* vec) {
    if (!vec) {
        return 0;
    }

    return vec->len;
}

size_t vector_capacity(const struct vector* vec) {
    if (!vec) {
        return 0;
    }

    return vec->capacity;
}

bool vector_is_empty(const struct vector* vec) {
    if (!vec) {
        return true;
    }

    return vec->len == 0;
}

void vector_clear(struct vector* vec) {
    if (!vec) {
        return;
    }

    vec->len = 0;
}

int vector_reserve(struct vector* vec, size_t capacity) {
    if (!vec) {
        return -EINVAL;
    }

    if (capacity <= vec->capacity) {
        return 0;
    }

    return ensure_capacity(vec, capacity);
}

int vector_shrink_to_fit(struct vector* vec) {
    if (!vec) {
        return -EINVAL;
    }

    if (vec->len == vec->capacity) {
        return 0;
    }

    if (vec->len == 0) {
        free((void*)vec->data);
        vec->data = nullptr;
        vec->capacity = 0;
        return 0;
    }

    void** new_data =
        (void**)realloc((void*)vec->data, vec->len * sizeof(void*));
    if (!new_data) {
        return -ENOMEM;
    }

    vec->data = new_data;
    vec->capacity = vec->len;

    return 0;
}

void vector_destroy(struct vector* vec) {
    if (!vec) {
        return;
    }

    free((void*)vec->data);
    vec->data = nullptr;
    vec->len = 0;
    vec->capacity = 0;
}

static int ensure_capacity(struct vector* vec, size_t required_capacity) {
    if (required_capacity <= vec->capacity) {
        return 0;
    }

    size_t new_capacity = 0;

    if (vec->capacity == 0) {
        new_capacity = VECTOR_INITIAL_CAPACITY;
    } else {
        new_capacity = vec->capacity * VECTOR_GROWTH_FACTOR;
    }

    if (new_capacity < required_capacity) {
        new_capacity = required_capacity;
    }

    void** new_data =
        (void**)realloc((void*)vec->data, new_capacity * sizeof(void*));
    if (!new_data) {
        return -ENOMEM;
    }

    vec->data = new_data;
    vec->capacity = new_capacity;

    return 0;
}

#include "compiler/ds/string_buffer.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const size_t SSO_SIZE = sizeof(((struct string_buffer*)0)->data.sso);

static char* get_content_ptr(struct string_buffer* sb);
static size_t get_capacity(const struct string_buffer* sb);
static int ensure_capacity(struct string_buffer* sb, size_t additional_len);

int string_buffer_init(struct string_buffer* sb) {
    if (!sb) {
        return -EINVAL;
    }

    sb->len = 0;
    sb->on_heap = false;
    sb->data.sso[0] = '\0';

    return 0;
}

void string_buffer_destroy(struct string_buffer* sb) {
    if (!sb) {
        return;
    }

    if (sb->on_heap) {
        free(sb->data.heap.ptr);
        sb->data.heap.ptr = nullptr;
    }

    string_buffer_init(sb);
}

int string_buffer_append(struct string_buffer* sb,
                         const char* str,
                         size_t len) {
    if (!sb || (!str && len > 0)) {
        return -EINVAL;
    }

    if (len == 0) {
        return 0;
    }

    int result = ensure_capacity(sb, len);
    if (result != 0) {
        return result;
    }

    char* dest = get_content_ptr(sb);
    memcpy(dest + sb->len, str, len);

    sb->len += len;
    dest[sb->len] = '\0';

    return 0;
}

int string_buffer_append_cstr(struct string_buffer* sb, const char* str) {
    if (!str) {
        return -EINVAL;
    }

    return string_buffer_append(sb, str, strlen(str));
}

const char* string_buffer_get_cstr(const struct string_buffer* sb) {
    if (!sb) {
        return "";
    }
    if (sb->on_heap) {
        return sb->data.heap.ptr;
    }
    return sb->data.sso;
}

size_t string_buffer_get_len(const struct string_buffer* sb) {
    if (!sb) {
        return 0;
    }

    return sb->len;
}

static char* get_content_ptr(struct string_buffer* sb) {
    if (sb->on_heap) {
        return sb->data.heap.ptr;
    }

    return sb->data.sso;
}

static size_t get_capacity(const struct string_buffer* sb) {
    if (sb->on_heap) {
        return sb->data.heap.capacity;
    }

    return SSO_SIZE;
}

static int ensure_capacity(struct string_buffer* sb, size_t additional_len) {
    const size_t required_len = sb->len + additional_len;
    const size_t current_capacity = get_capacity(sb);

    if (required_len < current_capacity) {
        return 0;
    }

    size_t new_capacity = current_capacity * 2;
    if (new_capacity < required_len + 1) {
        new_capacity = required_len + 1;
    }

    char* new_ptr = malloc(new_capacity);
    if (!new_ptr) {
        return -ENOMEM;
    }

    const char* old_ptr = get_content_ptr(sb);
    memcpy(new_ptr, old_ptr, sb->len);

    if (sb->on_heap) {
        free(sb->data.heap.ptr);
    }

    sb->on_heap = true;
    sb->data.heap.ptr = new_ptr;
    sb->data.heap.capacity = new_capacity;

    return 0;
}

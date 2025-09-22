#include "compiler/file_reader.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int file_reader_init(struct file_reader* reader, const char* filename) {
    if (!reader || !filename) {
        return -EINVAL;
    }

    reader->file = fopen(filename, "rb");
    if (!reader->file) {
        return -errno;
    }

    return 0;
}

int file_reader_read_all(struct file_reader* reader,
                         char** content,
                         size_t* len) {
    if (!reader || !content || !len) {
        return -EINVAL;
    }

    *content = nullptr;
    *len = 0;

    if (fseek(reader->file, 0, SEEK_END) != 0) {
        return -errno;
    }

    long file_size = ftell(reader->file);
    if (file_size < 0) {
        return -errno;
    }

    if (fseek(reader->file, 0, SEEK_SET) != 0) {
        return -errno;
    }

    *content = malloc((size_t)file_size + 1);
    if (!*content) {
        return -ENOMEM;
    }

    size_t bytes_read = fread(*content, 1, file_size, reader->file);
    if (bytes_read < (size_t)file_size) {
        free(*content);
        return ferror(reader->file) ? -errno : -EIO;
    }

    *content[(size_t)file_size] = '\0';
    *len = file_size;

    return 0;
}

void file_reader_destroy(struct file_reader* reader) {
    if (!reader || !reader->file) {
        return;
    }

    (void)fclose(reader->file);
    reader->file = nullptr;
}

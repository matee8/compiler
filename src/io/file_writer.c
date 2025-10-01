#include "compiler/io/file_writer.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

int file_writer_init(struct file_writer* writer, const char* filename) {
    if (!writer || !filename) {
        return -EINVAL;
    }

    writer->file = fopen(filename, "wb");
    if (!writer->file) {
        return -errno;
    }

    return 0;
}

int file_writer_write(struct file_writer* writer,
                      const char* content,
                      size_t len) {
    if (!writer || !content || !writer->file) {
        return -EINVAL;
    }

    if (len == 0) {
        return 0;
    }

    size_t bytes_written = fwrite(content, 1, len, writer->file);

    if (bytes_written < len) {
        return -EIO;
    }

    return 0;
}

void file_writer_destroy(struct file_writer* writer) {
    if (!writer || !writer->file) {
        return;
    }

    (void)fclose(writer->file);
    writer->file = nullptr;
}

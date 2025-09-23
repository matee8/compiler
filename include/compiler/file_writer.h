#ifndef COMPILER_FILE_WRITER_H
#define COMPILER_FILE_WRITER_H

#include <stdio.h>

struct file_writer {
    FILE* file;
};

int file_writer_init(struct file_writer* writer, const char* filename);
int file_writer_write(struct file_writer* writer,
                      const char* content,
                      size_t len);
void file_writer_destroy(struct file_writer* writer);

#endif

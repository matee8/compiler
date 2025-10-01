#ifndef COMPILER_IO_FILE_READER_H
#define COMPILER_IO_FILE_READER_H

#include <stdio.h>

struct file_reader {
    FILE* file;
};

int file_reader_init(struct file_reader* reader, const char* filename);
int file_reader_read_all(struct file_reader* reader,
                         char** content,
                         size_t* len);
void file_reader_destroy(struct file_reader* reader);

#endif

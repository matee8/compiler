#include "compiler/file_writer.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler/file_reader.h"

#define RUN_TEST(test)                          \
    do {                                        \
        printf("Running test: %s...\n", #test); \
        test();                                 \
    } while (0)

void test_writer_init_null_args(void) {
    struct file_writer writer;
    assert(file_writer_init(nullptr, "some_file.txt") == -EINVAL);
    assert(file_writer_init(&writer, nullptr) == -EINVAL);
}

void test_write_to_new_file(void) {
    const char* filename = "test_write_new.txt";
    const char* content_str = "This is a test.\nLine 2.";
    struct file_writer writer;

    assert(file_writer_init(&writer, filename) == 0);
    int write_ret =
        file_writer_write(&writer, content_str, strlen(content_str));
    assert(write_ret == 0);
    file_writer_destroy(&writer);

    struct file_reader reader;
    char* read_content = nullptr;
    size_t read_len = 0;

    assert(file_reader_init(&reader, filename) == 0);
    assert(file_reader_read_all(&reader, &read_content, &read_len) == 0);

    assert(read_len == strlen(content_str));
    assert(strcmp(content_str, read_content) == 0);

    free(read_content);
    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_write_truncates_existing_file(void) {
    const char* filename = "test_truncate.txt";
    const char* initial_content = "This is the original long content.";
    const char* new_content = "Short.";

    FILE* fp = fopen(filename, "w");
    assert(fp != NULL);
    (void)fputs(initial_content, fp);
    (void)fclose(fp);

    struct file_writer writer;
    assert(file_writer_init(&writer, filename) == 0);
    assert(file_writer_write(&writer, new_content, strlen(new_content)) == 0);
    file_writer_destroy(&writer);

    struct file_reader reader;
    char* read_content = nullptr;
    size_t read_len = 0;
    assert(file_reader_init(&reader, filename) == 0);
    assert(file_reader_read_all(&reader, &read_content, &read_len) == 0);

    assert(read_len == strlen(new_content));
    assert(strcmp(new_content, read_content) == 0);

    free(read_content);
    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_write_zero_bytes(void) {
    const char* filename = "test_write_zero.txt";
    struct file_writer writer;

    assert(file_writer_init(&writer, filename) == 0);
    assert(file_writer_write(&writer, "some data", 0) == 0);
    file_writer_destroy(&writer);

    struct file_reader reader;
    char* read_content = nullptr;
    size_t read_len = 0;
    assert(file_reader_init(&reader, filename) == 0);
    assert(file_reader_read_all(&reader, &read_content, &read_len) == 0);

    assert(read_len == 0);
    assert(read_content[0] == '\0');

    free(read_content);
    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_writer_destroy_safety(void) {
    const char* filename = "test_writer_destroy.txt";
    struct file_writer writer;

    FILE* fp = fopen(filename, "w");
    assert(fp != NULL);
    (void)fclose(fp);

    assert(file_writer_init(&writer, filename) == 0);

    file_writer_destroy(nullptr);
    file_writer_destroy(&writer);
    assert(writer.file == NULL);
    file_writer_destroy(&writer);

    (void)remove(filename);
}

int main(void) {
    puts("Starting file writer tests.\n");

    RUN_TEST(test_writer_init_null_args);
    RUN_TEST(test_write_to_new_file);
    RUN_TEST(test_write_truncates_existing_file);
    RUN_TEST(test_write_zero_bytes);
    RUN_TEST(test_writer_destroy_safety);

    puts("\nAll file writer tests passed successfully!");

    return EXIT_SUCCESS;
}

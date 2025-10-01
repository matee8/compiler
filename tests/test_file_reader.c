#include "compiler/io/file_reader.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TEST(test)                          \
    do {                                        \
        printf("Running test: %s...\n", #test); \
        test();                                 \
    } while (0)

void create_test_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    assert(file != NULL);
    if (content) {
        (void)fputs(content, file);
    }
    (void)fclose(file);
}

void test_init_null_args(void) {
    struct file_reader reader;
    assert(file_reader_init(nullptr, "some_file.txt") == -EINVAL);
    assert(file_reader_init(&reader, nullptr) == -EINVAL);
}

void test_init_non_existent_file(void) {
    struct file_reader reader;
    int ret = file_reader_init(&reader, "non_existent_file.txt");
    assert(ret == -ENOENT);
}

void test_read_regular_file(void) {
    const char* filename = "test_regular.txt";
    const char* content_str = "hello\nworld";
    create_test_file(filename, content_str);

    struct file_reader reader;
    char* content = nullptr;
    size_t len = 0;

    int ret_init = file_reader_init(&reader, filename);
    assert(ret_init == 0);

    int ret_read = file_reader_read_all(&reader, &content, &len);
    assert(ret_read == 0);
    assert(len == strlen(content_str));
    assert(strcmp(content, content_str) == 0);

    free(content);
    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_read_empty_file(void) {
    const char* filename = "test_empty.txt";
    create_test_file(filename, "");

    struct file_reader reader;
    char* content = nullptr;
    size_t len = 0;

    assert(file_reader_init(&reader, filename) == 0);

    int ret_read = file_reader_read_all(&reader, &content, &len);
    assert(ret_read == 0);
    assert(len == 0);
    assert(content != nullptr);
    assert(content[0] == '\0');

    free(content);
    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_read_all_null_args(void) {
    const char* filename = "test_null_args.txt";
    create_test_file(filename, "content");

    struct file_reader reader;
    char* content = nullptr;
    size_t len = 0;

    assert(file_reader_init(&reader, filename) == 0);

    assert(file_reader_read_all(nullptr, &content, &len) == -EINVAL);
    assert(file_reader_read_all(&reader, nullptr, &len) == -EINVAL);
    assert(file_reader_read_all(&reader, &content, nullptr) == -EINVAL);

    file_reader_destroy(&reader);
    (void)remove(filename);
}

void test_destroy_safety(void) {
    const char* filename = "test_destroy.txt";
    create_test_file(filename, "content");

    struct file_reader reader;
    assert(file_reader_init(&reader, filename) == 0);

    file_reader_destroy(nullptr);
    file_reader_destroy(&reader);
    assert(reader.file == nullptr);

    file_reader_destroy(&reader);

    (void)remove(filename);
}

int main(void) {
    puts("Starting file reader tests.\n");

    RUN_TEST(test_init_null_args);
    RUN_TEST(test_init_non_existent_file);
    RUN_TEST(test_read_regular_file);
    RUN_TEST(test_read_empty_file);
    RUN_TEST(test_read_all_null_args);
    RUN_TEST(test_destroy_safety);

    puts("\nAll file reader tests passedd successfully!");

    return EXIT_SUCCESS;
}

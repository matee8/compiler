#include "compiler/ds/vector.h"
#include "compiler/preprocessor/rule.h"
#include "compiler/preprocessor/rule_loader.h"

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

static void create_test_file(const char* name, const char* content) {
    FILE* fp = fopen(name, "w");
    assert(fp);
    (void)fputs(content, fp);
    (void)fclose(fp);
}

void test_load_succeeds_on_valid_file(void) {
    const char* filename = "valid_rules.txt";
    create_test_file(filename,
                     "//.*, \n"
                     "/\\*.*\\*/, \n"
                     "  , ");

    struct vector rules;
    int ret = rule_loader_load_from_file(filename, &rules);
    assert(ret == 0);
    assert(vector_len(&rules) == 3);

    struct rule* r0 = (struct rule*)vector_get(&rules, 0);
    assert(strcmp(r0->pattern, "//.*") == 0);
    assert(strcmp(r0->replacement, " ") == 0);

    struct rule* r1 = (struct rule*)vector_get(&rules, 1);
    assert(strcmp(r1->pattern, "/\\*.*\\*/") == 0);
    assert(strcmp(r1->replacement, " ") == 0);

    struct rule* r2 = (struct rule*)vector_get(&rules, 2);
    assert(strcmp(r2->pattern, "  ") == 0);
    assert(strcmp(r2->replacement, " ") == 0);

    rule_loader_free_rules(&rules);
    (void)remove(filename);
}

void test_load_succeeds_on_empty_file(void) {
    const char* filename = "empty_rules.txt";
    create_test_file(filename, "");

    struct vector rules;
    int ret = rule_loader_load_from_file(filename, &rules);
    assert(ret == 0);
    assert(vector_is_empty(&rules));

    rule_loader_free_rules(&rules);
    (void)remove(filename);
}

void test_load_skips_malformed_lines(void) {
    const char* filename = "malformed_rules.txt";
    create_test_file(filename,
                     "valid,rule\n"
                     "malformed line without comma\n"
                     "\n"
                     "another,valid\n");

    struct vector rules;
    int ret = rule_loader_load_from_file(filename, &rules);
    assert(ret == 0);
    assert(vector_len(&rules) == 2);

    struct rule* r0 = (struct rule*)vector_get(&rules, 0);
    assert(strcmp(r0->pattern, "valid") == 0);

    struct rule* r1 = (struct rule*)vector_get(&rules, 1);
    assert(strcmp(r1->pattern, "another") == 0);

    rule_loader_free_rules(&rules);
    (void)remove(filename);
}

void test_load_fails_on_nonexistent_file(void) {
    struct vector rules;
    int ret = rule_loader_load_from_file("nonexistent_file.txt", &rules);
    assert(ret == -ENOENT);
}

void test_load_fails_on_invalid_regex(void) {
    const char* filename = "invalid_regex.txt";
    create_test_file(filename, "[a-z,replacement\n");

    struct vector rules;
    int ret = rule_loader_load_from_file(filename, &rules);
    assert(ret == -EINVAL);
    assert(vector_is_empty(&rules));

    (void)remove(filename);
}

void test_load_fails_on_null_args(void) {
    struct vector rules;
    assert(rule_loader_load_from_file(nullptr, &rules) == -EINVAL);
    assert(rule_loader_load_from_file("file.txt", nullptr) == -EINVAL);
}

int main(void) {
    puts("Starting rule loader tests.\n");

    RUN_TEST(test_load_succeeds_on_valid_file);
    RUN_TEST(test_load_succeeds_on_empty_file);
    RUN_TEST(test_load_skips_malformed_lines);
    RUN_TEST(test_load_fails_on_nonexistent_file);
    RUN_TEST(test_load_fails_on_invalid_regex);
    RUN_TEST(test_load_fails_on_null_args);

    puts("\nAll rule loader tests passed successfully!");
    return EXIT_SUCCESS;
}

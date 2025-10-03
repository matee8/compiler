#include "compiler/ds/vector.h"
#include "compiler/preprocessor/pipeline.h"
#include "compiler/preprocessor/rule.h"
#include "compiler/preprocessor/rule_loader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TEST(test)                          \
    do {                                        \
        printf("Running test: %s...\n", #test); \
        test();                                 \
    } while (0)

static struct rule* create_rule(const char* pattern, const char* replacement) {
    struct rule* r = malloc(sizeof(*r));
    assert(r);
    r->pattern = strdup(pattern);
    r->replacement = strdup(replacement);
    assert(r->pattern && r->replacement);
    assert(regcomp(&r->compiled_regex, pattern, REG_EXTENDED) == 0);
    return r;
}

void test_run_with_no_rules(void) {
    struct vector rules;
    vector_init(&rules);
    char* output = nullptr;

    int ret = preprocessor_run("int main() { return 0; }", &rules, &output);
    assert(ret == 0);
    assert(strcmp(output, "int main() { return 0; }") == 0);

    free(output);
    vector_destroy(&rules);
}

void test_run_with_simple_replacement(void) {
    struct vector rules;
    vector_init(&rules);
    vector_push(&rules, create_rule("int", "long"));
    char* output = nullptr;

    int ret = preprocessor_run("int main(int argc);", &rules, &output);
    assert(ret == 0);
    assert(strcmp(output, "long main(long argc);") == 0);

    free(output);
    rule_loader_free_rules(&rules);
}

void test_run_chooses_earliest_match(void) {
    struct vector rules;
    vector_init(&rules);
    vector_push(&rules, create_rule("main", "start"));
    vector_push(&rules, create_rule("int", "long"));

    char* output = nullptr;
    int ret = preprocessor_run("int main();", &rules, &output);
    assert(ret == 0);
    assert(strcmp(output, "long start();") == 0);

    free(output);
    rule_loader_free_rules(&rules);
}

void test_run_with_empty_replacement(void) {
    struct vector rules;
    vector_init(&rules);

    vector_push(&rules, create_rule("/\\*([^*]|\\*+[^*/])*\\*+/", ""));

    char* output = nullptr;
    const char* input = "start /* this is a comment */ end";
    int ret = preprocessor_run(input, &rules, &output);
    assert(ret == 0);
    assert(strcmp(output, "start  end") == 0);

    free(output);
    rule_loader_free_rules(&rules);
}

void test_run_on_empty_input(void) {
    struct vector rules;
    vector_init(&rules);
    vector_push(&rules, create_rule("a", "b"));

    char* output = nullptr;
    int ret = preprocessor_run("", &rules, &output);
    assert(ret == 0);
    assert(strcmp(output, "") == 0);

    free(output);
    rule_loader_free_rules(&rules);
}

int main(void) {
    puts("Starting preprocessor pipeline tests.\n");

    RUN_TEST(test_run_with_no_rules);
    RUN_TEST(test_run_with_simple_replacement);
    RUN_TEST(test_run_chooses_earliest_match);
    RUN_TEST(test_run_with_empty_replacement);
    RUN_TEST(test_run_on_empty_input);

    puts("\nAll preprocessor pipeline tests passed successfully!");
    return EXIT_SUCCESS;
}

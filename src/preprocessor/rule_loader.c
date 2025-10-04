#include "compiler/preprocessor/rule_loader.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pcre2.h>

#include "compiler/ds/vector.h"
#include "compiler/io/file_reader.h"
#include "compiler/preprocessor/rule.h"

static void free_single_rule(struct rule* r);
static int parse_line_as_rule(char* line, struct rule** out_rule);

int rule_loader_load_from_file(const char* filename, struct vector* rules) {
    if (!filename || !rules) {
        return -EINVAL;
    }

    int ret = 0;

    if (vector_init(rules) != 0) {
        return -ENOMEM;
    }

    struct file_reader reader = {.file = nullptr};

    ret = file_reader_init(&reader, filename);
    if (ret != 0) {
        goto cleanup_vector;
    }

    char* content = nullptr;
    size_t content_len = 0;
    ret = file_reader_read_all(&reader, &content, &content_len);
    if (ret != 0) {
        goto cleanup_reader;
    }

    char* line_saveptr = nullptr;
    char* line = strtok_r(content, "\n", &line_saveptr);
    while (line != nullptr) {
        struct rule* new_rule = nullptr;
        ret = parse_line_as_rule(line, &new_rule);

        if (ret == -ENOENT) {
            line = strtok_r(nullptr, "\n", &line_saveptr);
            continue;
        }

        if (ret != 0) {
            break;
        }

        ret = vector_push(rules, new_rule);
        if (ret != 0) {
            free_single_rule(new_rule);
            break;
        }

        line = strtok_r(nullptr, "\n", &line_saveptr);
    }

    free(content);

cleanup_reader:
    file_reader_destroy(&reader);

cleanup_vector:
    if (ret != 0) {
        rule_loader_free_rules(rules);
    }

    return ret;
}

void rule_loader_free_rules(struct vector* rules) {
    if (!rules) {
        return;
    }

    for (size_t i = 0; i < vector_len(rules); ++i) {
        struct rule* r = (struct rule*)vector_get(rules, i);
        free_single_rule(r);
    }

    vector_destroy(rules);
}

static int parse_line_as_rule(char* line, struct rule** out_rule) {
    if (strspn(line, " \t\r") == strlen(line)) {
        return -ENOENT;
    }

    char* comma = strrchr(line, ',');
    if (!comma) {
        (void)fprintf(stderr, "Skipping malformed rule (no comma): %s\n", line);
        return -ENOENT;
    }

    *comma = '\0';
    const char* pattern_str = line;
    const char* replacement_str = comma + 1;

    struct rule* new_rule = malloc(sizeof(*new_rule));
    if (!new_rule) {
        return -ENOMEM;
    }

    memset(new_rule, 0, sizeof(*new_rule));

    new_rule->pattern = strdup(pattern_str);
    new_rule->replacement = strdup(replacement_str);

    if (!new_rule->pattern || !new_rule->replacement) {
        free_single_rule(new_rule);
        return -ENOMEM;
    }

    int error_code = 0;
    PCRE2_SIZE error_offset = 0;
    new_rule->compiled_regex =
        pcre2_compile((PCRE2_SPTR)new_rule->pattern, PCRE2_ZERO_TERMINATED,
                      PCRE2_UCP, &error_code, &error_offset, nullptr);

    if (new_rule->compiled_regex == nullptr) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_code, buffer, sizeof(buffer));
        (void)fprintf(stderr, "Failed to compile regex at offset %zu: %s\n",
                      error_offset, (char*)buffer);
        free_single_rule(new_rule);
        return -EINVAL;
    }

    *out_rule = new_rule;
    return 0;
}

static void free_single_rule(struct rule* r) {
    if (!r) {
        return;
    }
    if (r->compiled_regex) {
        pcre2_code_free(r->compiled_regex);
    }
    free(r->pattern);
    free(r->replacement);
    free(r);
}

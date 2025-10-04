#include "compiler/preprocessor/pipeline.h"

#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <pcre2.h>

#include "compiler/ds/string_buffer.h"
#include "compiler/ds/vector.h"
#include "compiler/preprocessor/rule.h"

struct best_match {
    const struct rule* rule;
    regmatch_t pmatch;
    bool is_found;
};

static struct best_match find_earliest_match(const char* search_pos,
                                             const struct vector* rules);

static int process_match(struct string_buffer* sb,
                         const char** current_pos,
                         const struct best_match* match);

int preprocessor_run(const char* input_content,
                     const struct vector* rules,
                     char** output_content) {
    if (!input_content || !rules || !output_content) {
        return -EINVAL;
    }

    *output_content = nullptr;

    int ret = 0;
    struct string_buffer sb;
    ret = string_buffer_init(&sb);
    if (ret != 0) {
        return ret;
    }

    const char* current_pos = input_content;

    while (*current_pos != '\0') {
        const struct best_match match = find_earliest_match(current_pos, rules);

        if (match.is_found) {
            ret = process_match(&sb, &current_pos, &match);

            if (ret != 0) {
                goto cleanup_sb;
            }
        } else {
            ret = string_buffer_append_cstr(&sb, current_pos);

            if (ret != 0) {
                goto cleanup_sb;
            }

            break;
        }
    }

    *output_content = string_buffer_to_cstr(&sb);
    if (!*output_content) {
        ret = -ENOMEM;
        goto cleanup_sb;
    }

    return 0;

cleanup_sb:
    string_buffer_destroy(&sb);
    return ret;
}

static struct best_match find_earliest_match(const char* search_pos,
                                             const struct vector* rules) {
    struct best_match match = {.is_found = false, .pmatch = {.rm_so = INT_MAX}};

    pcre2_match_data* match_data = nullptr;

    for (size_t i = 0; i < vector_len(rules); ++i) {
        const struct rule* r = (const struct rule*)vector_get(rules, i);

        if (match_data == nullptr) {
            match_data = pcre2_match_data_create_from_pattern(r->compiled_regex,
                                                              nullptr);

            if (match_data == nullptr) {
                break;
            }
        }

        int rc = pcre2_match(r->compiled_regex, (PCRE2_SPTR)search_pos,
                             strlen(search_pos), 0, PCRE2_NOTBOL, match_data,
                             nullptr);

        if (rc >= 0) {
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);

            if (ovector[0] < (size_t)match.pmatch.rm_so) {
                match.pmatch.rm_so = (regoff_t)ovector[0];
                match.pmatch.rm_eo = (regoff_t)ovector[1];
                match.rule = r;
                match.is_found = true;
            }
        }
    }

    if (match_data) {
        pcre2_match_data_free(match_data);
    }

    return match;
}

static int process_match(struct string_buffer* sb,
                         const char** current_pos,
                         const struct best_match* match) {
    int ret = 0;

    if (match->pmatch.rm_so > 0) {
        ret =
            string_buffer_append(sb, *current_pos, (size_t)match->pmatch.rm_so);
        if (ret != 0) {
            return ret;
        }
    }

    ret = string_buffer_append_cstr(sb, match->rule->replacement);
    if (ret != 0) {
        return ret;
    }

    const char* next_pos = *current_pos + match->pmatch.rm_eo;

    if (next_pos == *current_pos && *next_pos != '\0') {
        ret = string_buffer_append(sb, next_pos, 1);
        if (ret != 0) {
            return ret;
        }

        next_pos++;
    }

    *current_pos = next_pos;
    return 0;
}

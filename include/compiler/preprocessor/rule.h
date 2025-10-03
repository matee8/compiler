#ifndef COMPILER_PREPROCESSOR_RULE_H
#define COMPILER_PREPROCESSOR_RULE_H

#include <regex.h>

struct rule {
    char* pattern;
    char* replacement;
    regex_t compiled_regex;
}

#endif

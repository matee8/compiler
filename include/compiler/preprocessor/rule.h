#ifndef COMPILER_PREPROCESSOR_RULE_H
#define COMPILER_PREPROCESSOR_RULE_H

#include "pcre2.h"

struct rule {
    char* pattern;
    char* replacement;
    pcre2_code* compiled_regex;
};

#endif

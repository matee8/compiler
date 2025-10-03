#ifndef COMPILER_PREPROCESSOR_RULE_LOADER_H
#define COMPILER_PREPROCESSOR_RULE_LOADER_H

#include "compiler/ds/vector.h"

int rule_loader_load_from_file(const char* filename, struct vector* rules);
void rule_loader_free_rules(struct vector* rules);

#endif

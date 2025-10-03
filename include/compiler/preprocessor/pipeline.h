#ifndef COMPILER_PREPROCESSOR_PIPELINE_H
#define COMPILER_PREPROCESSOR_PIPELINE_H

#include "compiler/ds/vector.h"

int preprocessor_run(const char* input_content,
                     const struct vector* rules,
                     char** output_content);

#endif

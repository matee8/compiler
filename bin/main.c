#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler/io/file_reader.h"
#include "compiler/io/file_writer.h"
#include "compiler/preprocessor/pipeline.h"
#include "compiler/preprocessor/rule_loader.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        (void)fprintf(stderr,
                      "Usage: %s <rules_file> <input_file> <output_file>\n",
                      argv[0]);
        return EXIT_FAILURE;
    }

    const char* rules_filename = argv[1];
    const char* input_filename = argv[2];
    const char* output_filename = argv[3];

    int exit_code = EXIT_SUCCESS;
    int ret = 0;

    struct vector rules;
    vector_init(&rules);

    ret = rule_loader_load_from_file(rules_filename, &rules);
    if (ret != 0) {
        (void)fprintf(stderr,
                      "Error: Failed to load or compile rules (code: %d).\n",
                      ret);
        exit_code = EXIT_FAILURE;
        goto cleanup_rules;
    }

    struct file_reader reader;
    ret = file_reader_init(&reader, input_filename);
    if (ret != 0) {
        (void)fprintf(stderr,
                      "Error: Failed to open input file '%s' (code: %d).\n",
                      input_filename, ret);
        exit_code = EXIT_FAILURE;
        goto cleanup_rules;
    }

    char* input_content = nullptr;
    size_t input_len = 0;
    ret = file_reader_read_all(&reader, &input_content, &input_len);
    if (ret != 0) {
        (void)fprintf(stderr, "Error: Failed to read input file (code: %d).\n",
                      ret);
        file_reader_destroy(&reader);
        exit_code = EXIT_FAILURE;
        goto cleanup_input_content;
    }
    file_reader_destroy(&reader);

    char* output_content = nullptr;

    ret = preprocessor_run(input_content, &rules, &output_content);
    if (ret != 0) {
        (void)fprintf(
            stderr, "Error: Preprocessor failed during execution (code: %d).\n",
            ret);
        exit_code = EXIT_FAILURE;
        goto cleanup_output_content;
    }

    struct file_writer writer;
    ret = file_writer_init(&writer, output_filename);
    if (ret != 0) {
        (void)fprintf(stderr,
                      "Error: Failed to open output file '%s' (code: %d).\n",
                      output_filename, ret);
        exit_code = EXIT_FAILURE;
        goto cleanup_output_content;
    }

    ret = file_writer_write(&writer, output_content, strlen(output_content));
    if (ret != 0) {
        (void)fprintf(
            stderr, "Error: Failed to write to output file (code: %d).\n", ret);
        file_writer_destroy(&writer);
        exit_code = EXIT_FAILURE;
        goto cleanup_output_content;
    }
    file_writer_destroy(&writer);

cleanup_output_content:
    free(output_content);
cleanup_input_content:
    free(input_content);
cleanup_rules:
    rule_loader_free_rules(&rules);

    return exit_code;
}

/* 3. gyakorlat
 *
 * Lexer osztály:
 *   - String content: "IF a==2 THEN b=20 ELSE b=12"
 *   - List<String> symbolTable
 *   - int symbolTableIndex
 *   - Dictionary<string, string> replaces
 *   - void replaceKeywords(void)
 *   - String createSymbolTableIndex(string symbol)
 */

#include "compiler/ds/vector.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define RUN_TEST(test)                          \
    do {                                        \
        printf("Running test: %s...\n", #test); \
        test();                                 \
    } while (0)

void test_init_success(void) {
    struct vector vec;
    int ret = vector_init(&vec);

    assert(ret == 0);
    assert(vector_len(&vec) == 0);
    assert(vector_capacity(&vec) == 0);
    assert(vector_is_empty(&vec));

    vector_destroy(&vec);
}

void test_init_null_arg(void) {
    assert(vector_init(nullptr) == -EINVAL);
}

void test_init_with_capacity_success(void) {
    struct vector vec;
    int ret = vector_init_with_capacity(&vec, 16);

    assert(ret == 0);
    assert(vector_len(&vec) == 0);
    assert(vector_capacity(&vec) == 16);
    assert(vector_is_empty(&vec));

    vector_destroy(&vec);
}

void test_init_with_capacity_zero(void) {
    struct vector vec;
    int ret = vector_init_with_capacity(&vec, 0);

    assert(ret == 0);
    assert(vector_len(&vec) == 0);
    assert(vector_capacity(&vec) == 0);

    vector_destroy(&vec);
}

void test_init_with_capacity_null_arg(void) {
    assert(vector_init_with_capacity(nullptr, 10) == -EINVAL);
}

void test_push_single_element(void) {
    struct vector vec;
    int value = 42;

    assert(vector_init(&vec) == 0);

    int ret = vector_push(&vec, &value);
    assert(ret == 0);
    assert(vector_len(&vec) == 1);
    assert(vector_get(&vec, 0) == &value);

    vector_destroy(&vec);
}

void test_push_multiple_elements(void) {
    struct vector vec;
    int values[] = {1, 2, 3, 4, 5};
    size_t count = sizeof(values) / sizeof(values[0]);

    assert(vector_init(&vec) == 0);

    // NOLINTNEXTLINE
    for (size_t i = 0; i < count; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_len(&vec) == count);

    // NOLINTNEXTLINE
    for (size_t i = 0; i < count; i++) {
        assert(vector_get(&vec, i) == &values[i]);
    }

    vector_destroy(&vec);
}

void test_push_triggers_growth(void) {
    struct vector vec;
    int values[20];

    assert(vector_init(&vec) == 0);

    for (size_t i = 0; i < 20; i++) {
        values[i] = (int)i;
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_len(&vec) == 20);
    assert(vector_capacity(&vec) >= 20);

    for (size_t i = 0; i < 20; i++) {
        assert(vector_get(&vec, i) == &values[i]);
    }

    vector_destroy(&vec);
}

void test_push_null_vector(void) {
    int value = 42;
    assert(vector_push(nullptr, &value) == -EINVAL);
}

void test_pop_single_element(void) {
    struct vector vec;
    int value = 42;

    assert(vector_init(&vec) == 0);
    assert(vector_push(&vec, &value) == 0);

    void* popped = vector_pop(&vec);
    assert(popped == &value);
    assert(vector_len(&vec) == 0);
    assert(vector_is_empty(&vec));

    vector_destroy(&vec);
}

void test_pop_multiple_elements(void) {
    struct vector vec;
    int values[] = {1, 2, 3};

    assert(vector_init(&vec) == 0);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    for (int i = 2; i >= 0; i--) {
        void* popped = vector_pop(&vec);
        assert(popped == &values[i]);
    }

    assert(vector_is_empty(&vec));

    vector_destroy(&vec);
}

void test_pop_empty_vector(void) {
    struct vector vec;

    assert(vector_init(&vec) == 0);
    assert(vector_pop(&vec) == nullptr);

    vector_destroy(&vec);
}

void test_pop_null_vector(void) {
    assert(vector_pop(nullptr) == nullptr);
}

void test_get_valid_index(void) {
    struct vector vec;
    int values[] = {10, 20, 30};

    assert(vector_init(&vec) == 0);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_get(&vec, 0) == &values[0]);
    assert(vector_get(&vec, 1) == &values[1]);
    assert(vector_get(&vec, 2) == &values[2]);

    vector_destroy(&vec);
}

void test_get_out_of_bounds(void) {
    struct vector vec;
    int value = 42;

    assert(vector_init(&vec) == 0);
    assert(vector_push(&vec, &value) == 0);

    assert(vector_get(&vec, 1) == nullptr);
    assert(vector_get(&vec, 100) == nullptr);

    vector_destroy(&vec);
}

void test_get_null_vector(void) {
    assert(vector_get(nullptr, 0) == nullptr);
}

void test_set_valid_index(void) {
    struct vector vec;
    int values[] = {1, 2, 3};
    int new_value = 99;

    assert(vector_init(&vec) == 0);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_set(&vec, 1, &new_value) == 0);
    assert(vector_get(&vec, 1) == &new_value);

    vector_destroy(&vec);
}

void test_set_out_of_bounds(void) {
    struct vector vec;
    int value = 42;
    int new_value = 99;

    assert(vector_init(&vec) == 0);
    assert(vector_push(&vec, &value) == 0);

    assert(vector_set(&vec, 1, &new_value) == -EINVAL);
    assert(vector_set(&vec, 100, &new_value) == -EINVAL);

    vector_destroy(&vec);
}

void test_set_null_vector(void) {
    int value = 42;
    assert(vector_set(nullptr, 0, &value) == -EINVAL);
}

void test_len_operations(void) {
    struct vector vec;
    int values[] = {1, 2, 3};

    assert(vector_len(nullptr) == 0);
    assert(vector_init(&vec) == 0);
    assert(vector_len(&vec) == 0);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
        assert(vector_len(&vec) == i + 1);
    }

    vector_destroy(&vec);
}

void test_capacity_operations(void) {
    struct vector vec;

    assert(vector_capacity(nullptr) == 0);
    assert(vector_init(&vec) == 0);
    assert(vector_capacity(&vec) == 0);

    vector_destroy(&vec);

    assert(vector_init_with_capacity(&vec, 32) == 0);
    assert(vector_capacity(&vec) == 32);

    vector_destroy(&vec);
}

void test_is_empty_operations(void) {
    struct vector vec;
    int value = 42;

    assert(vector_is_empty(nullptr) == true);
    assert(vector_init(&vec) == 0);
    assert(vector_is_empty(&vec) == true);

    assert(vector_push(&vec, &value) == 0);
    assert(vector_is_empty(&vec) == false);

    (void)vector_pop(&vec);
    assert(vector_is_empty(&vec) == true);

    vector_destroy(&vec);
}

void test_clear_operations(void) {
    struct vector vec;
    int values[] = {1, 2, 3, 4, 5};

    assert(vector_init(&vec) == 0);

    for (size_t i = 0; i < 5; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    size_t capacity_before = vector_capacity(&vec);
    vector_clear(&vec);

    assert(vector_len(&vec) == 0);
    assert(vector_is_empty(&vec) == true);
    assert(vector_capacity(&vec) == capacity_before);

    vector_destroy(&vec);
}

void test_clear_null_vector(void) {
    vector_clear(nullptr);
}

void test_reserve_increase_capacity(void) {
    struct vector vec;

    assert(vector_init(&vec) == 0);
    assert(vector_reserve(&vec, 50) == 0);
    assert(vector_capacity(&vec) >= 50);
    assert(vector_len(&vec) == 0);

    vector_destroy(&vec);
}

void test_reserve_already_sufficient(void) {
    struct vector vec;

    assert(vector_init_with_capacity(&vec, 100) == 0);

    size_t capacity_before = vector_capacity(&vec);
    assert(vector_reserve(&vec, 50) == 0);
    assert(vector_capacity(&vec) == capacity_before);

    vector_destroy(&vec);
}

void test_reserve_null_vector(void) {
    assert(vector_reserve(nullptr, 10) == -EINVAL);
}

void test_shrink_to_fit_reduces_capacity(void) {
    struct vector vec;
    int values[] = {1, 2, 3};

    assert(vector_init_with_capacity(&vec, 100) == 0);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_capacity(&vec) == 100);
    assert(vector_shrink_to_fit(&vec) == 0);
    assert(vector_capacity(&vec) == 3);
    assert(vector_len(&vec) == 3);

    for (size_t i = 0; i < 3; i++) {
        assert(vector_get(&vec, i) == &values[i]);
    }

    vector_destroy(&vec);
}

void test_shrink_to_fit_empty_vector(void) {
    struct vector vec;

    assert(vector_init_with_capacity(&vec, 50) == 0);
    assert(vector_shrink_to_fit(&vec) == 0);
    assert(vector_capacity(&vec) == 0);
    assert(vector_len(&vec) == 0);

    vector_destroy(&vec);
}

void test_shrink_to_fit_null_vector(void) {
    assert(vector_shrink_to_fit(nullptr) == -EINVAL);
}

void test_destroy_safety(void) {
    struct vector vec;

    vector_destroy(nullptr);

    assert(vector_init(&vec) == 0);
    vector_destroy(&vec);

    assert(vec.data == nullptr);
    assert(vec.len == 0);
    assert(vec.capacity == 0);

    vector_destroy(&vec);
}

void test_workflow_integration(void) {
    struct vector vec;
    int values[100];

    assert(vector_init_with_capacity(&vec, 10) == 0);

    for (size_t i = 0; i < 100; i++) {
        values[i] = (int)i * 2;
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_len(&vec) == 100);

    for (size_t i = 0; i < 100; i++) {
        int* retrieved = vector_get(&vec, i);
        assert(retrieved == &values[i]);
        assert(*retrieved == (int)i * 2);
    }

    for (size_t i = 0; i < 50; i++) {
        (void)vector_pop(&vec);
    }

    assert(vector_len(&vec) == 50);

    vector_clear(&vec);
    assert(vector_is_empty(&vec) == true);

    for (size_t i = 0; i < 5; i++) {
        assert(vector_push(&vec, &values[i]) == 0);
    }

    assert(vector_shrink_to_fit(&vec) == 0);
    assert(vector_capacity(&vec) == 5);

    vector_destroy(&vec);
}

// NOLINTNEXTLINE
int main(void) {
    puts("Starting vector tests.\n");

    RUN_TEST(test_init_null_arg);
    RUN_TEST(test_init_with_capacity_success);
    RUN_TEST(test_init_with_capacity_zero);
    RUN_TEST(test_init_with_capacity_null_arg);
    RUN_TEST(test_push_single_element);
    RUN_TEST(test_push_multiple_elements);
    RUN_TEST(test_push_triggers_growth);
    RUN_TEST(test_push_null_vector);
    RUN_TEST(test_pop_single_element);
    RUN_TEST(test_pop_multiple_elements);
    RUN_TEST(test_pop_empty_vector);
    RUN_TEST(test_pop_null_vector);
    RUN_TEST(test_get_valid_index);
    RUN_TEST(test_get_out_of_bounds);
    RUN_TEST(test_get_null_vector);
    RUN_TEST(test_set_valid_index);
    RUN_TEST(test_set_out_of_bounds);
    RUN_TEST(test_set_null_vector);
    RUN_TEST(test_len_operations);
    RUN_TEST(test_capacity_operations);
    RUN_TEST(test_is_empty_operations);
    RUN_TEST(test_clear_operations);
    RUN_TEST(test_clear_null_vector);
    RUN_TEST(test_reserve_increase_capacity);
    RUN_TEST(test_reserve_already_sufficient);
    RUN_TEST(test_reserve_null_vector);
    RUN_TEST(test_shrink_to_fit_reduces_capacity);
    RUN_TEST(test_shrink_to_fit_empty_vector);
    RUN_TEST(test_shrink_to_fit_null_vector);
    RUN_TEST(test_destroy_safety);
    RUN_TEST(test_workflow_integration);

    puts("\nAll vector tests passed successfully!");

    return EXIT_SUCCESS;
}

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "converters/binary_converter.h"
#include "converters/shunting_yard.h"
#include "data_structs/int_stack.h"
#include "data_structs/stack.h"
#include "rpn_evaluator/evaluation.h"
#include "table_builders_for_webpage/table_builders.h"
#include "utils/find_nr_of_vars.h"

// Tests for int_to_binary_string function
void test_int_to_binary_string_invalid_input(void)
{
    char *res;

    res = int_to_binary_string(0, -1);
    CU_ASSERT_PTR_NULL(res);

    res = int_to_binary_string(0, -2);
    CU_ASSERT_PTR_NULL(res);

    res = int_to_binary_string(-1, 3);
    CU_ASSERT_PTR_NULL(res);

    res = int_to_binary_string(2, 1);
    CU_ASSERT_PTR_NULL(res);
}

void test_int_to_binary_string_valid_input(void)
{
    char *res;

    res = int_to_binary_string(1, 1);
    CU_ASSERT_STRING_EQUAL(res, "1");
    free(res);

    res = int_to_binary_string(2, 2);
    CU_ASSERT_STRING_EQUAL(res, "10");
    free(res);

    res = int_to_binary_string(1, 3);
    CU_ASSERT_STRING_EQUAL(res, "001");
    free(res);

    res = int_to_binary_string(3, 6);
    CU_ASSERT_STRING_EQUAL(res, "000011");
    free(res);

    res = int_to_binary_string(33554432, 26);
    CU_ASSERT_STRING_EQUAL(res, "10000000000000000000000000");
    free(res);

    res = int_to_binary_string(67108863, 26);
    CU_ASSERT_STRING_EQUAL(res, "11111111111111111111111111");
    free(res);
}

// Tests for replace_with_binary function
void test_replace_with_binary_invalid_input(void)
{
    char *res;

    res = replace_with_binary("ab", 4);
    CU_ASSERT_PTR_NULL(res);
}

void test_replace_with_binary_valid_input(void)
{
    char *res;

    res = replace_with_binary("ab", 2);
    CU_ASSERT_STRING_EQUAL(res, "10");
    free(res);

    res = replace_with_binary("ababab", 2);
    CU_ASSERT_STRING_EQUAL(res, "101010");
    free(res);

    res = replace_with_binary("ababab|||1,./';[]2345", 2);
    CU_ASSERT_STRING_EQUAL(res, "101010|||1,./';[]2345");
    free(res);

    res = replace_with_binary("a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z", 67108863);
    CU_ASSERT_STRING_EQUAL(res, "1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1");
    free(res);

    res = replace_with_binary("a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z", 1);
    CU_ASSERT_STRING_EQUAL(res, "0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1");
    free(res);
}

// Tests for convert_evaled_rpn_to_infix function
void test_convert_evaled_rpn_to_infix_invalid_input(void)
{
    char *res;
    int testMap[] = {1, 0, 3, 1};

    res = convert_evaled_rpn_to_infix(testMap, "0101", -1);
    CU_ASSERT_PTR_NULL(res);

    testMap[3] = -2;
    res = convert_evaled_rpn_to_infix(testMap, "0101", 4);
    CU_ASSERT_PTR_NULL(res);
}

void test_convert_evaled_rpn_to_infix_valid_input(void)
{
    char *res;
    int testMap[] = {1, 0, 3, 2};

    res = convert_evaled_rpn_to_infix(testMap, "0101", 4);
    CU_ASSERT_STRING_EQUAL(res, "1010");
    free(res);
}

// Tests for shunting_yard function
void test_shunting_yard_invalid_input(void)
{
    char *res;

    res = shunting_yard("A");
    CU_ASSERT_PTR_NULL(res);

    res = shunting_yard("|a");
    CU_ASSERT_PTR_NULL(res);

    res = shunting_yard("|a");
    CU_ASSERT_PTR_NULL(res);

    res = shunting_yard("-|a");
    CU_ASSERT_PTR_NULL(res);
}

void test_shunting_yard_valid_input(void)
{
    char *res;

    res = shunting_yard("a|b|c|d|e");
    CU_ASSERT_STRING_EQUAL(res, "ab|c|d|e|");
    free(res);

    res = shunting_yard("a");
    CU_ASSERT_STRING_EQUAL(res, "a");
    free(res);

    res = shunting_yard("--a");
    CU_ASSERT_STRING_EQUAL(res, "a--");
    free(res);

    res = shunting_yard("a|-b&c");
    CU_ASSERT_STRING_EQUAL(res, "ab-c&|");
    free(res);
}

// Tests for infix_map function
void test_infix_map(void)
{
    int expected_inf_map[] = {0, 2, 1, 4, 3, 6, 5};
    int expected_size = 7;
    int *inf_map = infix_map("a|b|c|d|e");

    for (int i = 0; i < expected_size; i++)
    {
        CU_ASSERT_EQUAL(inf_map[i], expected_inf_map[i]);
    }
}

// Test function for IntStack
void test_int_stack_operations(void)
{
    IntStack stack;
    int popped_value;

    // Initialize stack
    int_stack_init(&stack);
    CU_ASSERT_TRUE(stack.top == -1); // Stack should be empty

    // Push elements
    for (int i = 0; i < 5; i++)
    {
        CU_ASSERT_EQUAL(int_stack_peek(&stack), i);
    }

    // Pop elements and check values
    for (int i = 4; i >= 0; i--)
    {
        popped_value = int_stack_pop(&stack);
        CU_ASSERT_EQUAL(popped_value, i);
    }

    CU_ASSERT_TRUE(stack.top == -1); // Stack should be empty

    // Check underflow
    CU_ASSERT_PTR_NULL(int_stack_pop(&stack)); // Should return NULL on underflow

    // Check peek on empty stack
    CU_ASSERT_PTR_NULL(int_stack_peek(&stack)); // Should return NULL when empty
}

void test_stack_operations(void)
{
    Stack stack;
    char popped_value;

    // Initialize stack
    init_stack(&stack);
    CU_ASSERT_TRUE(is_empty(&stack)); // Stack should be empty

    // Push elements
    for (char i = 'a'; i <= 'e'; i++)
    {
        push(&stack, i);
        CU_ASSERT_FALSE(is_empty(&stack));
        CU_ASSERT_EQUAL(peek(&stack), i); // Check top item
    }

    // Pop elements and check values
    for (char i = 'e'; i >= 'a'; i--)
    {
        popped_value = pop(&stack);
        CU_ASSERT_EQUAL(popped_value, i); // Check popped value
    }

    CU_ASSERT_TRUE(is_empty(&stack)); // Stack should be empty

    // Check underflow
    CU_ASSERT_PTR_NULL(pop(&stack)); // Should return NULL on underflow

    // Check peek on empty stack
    CU_ASSERT_TRUE(is_empty(&stack));
}

void test_evaluate_expr(void)
{
    char *result;

    // Test valid expressions
    result = evaluate_expr("1 1 |");
    CU_ASSERT_STRING_EQUAL(result, "    1");
    free(result);

    result = evaluate_expr("1 0 &");
    CU_ASSERT_STRING_EQUAL(result, "    0");
    free(result);

    result = evaluate_expr("1 0 #");
    CU_ASSERT_STRING_EQUAL(result, "    1");
    free(result);

    result = evaluate_expr("1 1 >");
    CU_ASSERT_STRING_EQUAL(result, "    1");
    free(result);

    result = evaluate_expr("0 0 >");
    CU_ASSERT_STRING_EQUAL(result, "    1");
    free(result);

    result = evaluate_expr("1 -");
    CU_ASSERT_STRING_EQUAL(result, "  0");
    free(result);

    // Test more complex expression
    result = evaluate_expr("1 0 | 1 &");
    CU_ASSERT_STRING_EQUAL(result, "    1   1");
    free(result);

    result = evaluate_expr("0 1 | 0 & 1 |");
    CU_ASSERT_STRING_EQUAL(result, "    1   0   1");
    free(result);

    // Test invalid expressions
    result = evaluate_expr("1 1");
    CU_ASSERT_PTR_NULL(result); // Should return NULL for incomplete expression

    result = evaluate_expr("1 2 |");
    CU_ASSERT_PTR_NULL(result); // Should return NULL for unknown operator

    result = evaluate_expr("1 |");
    CU_ASSERT_PTR_NULL(result); // Should return NULL for bad expression

    // Test empty expression
    result = evaluate_expr("");
    CU_ASSERT_PTR_NULL(result); // Should return NULL for empty expression
}

void test_generate_postfix_row(void)
{
    char *result;
    // Test with a simple expression with two variables
    result = generate_postfix_row(0, 2, "ab&", 3);
    CU_ASSERT_STRING_EQUAL(result, "0 0 :   0 :   0\n");
    free(result);

    result = generate_postfix_row(1, 2, "ab&", 3);
    CU_ASSERT_STRING_EQUAL(result, "0 1 :   0 :   0\n");
    free(result);

    result = generate_postfix_row(3, 2, "ab&", 3);
    CU_ASSERT_STRING_EQUAL(result, "1 1 :   1 :   1\n");
    free(result);

    // Test with a single variable (no operator)
    result = generate_postfix_row(1, 1, "a", 1);
    CU_ASSERT_STRING_EQUAL(result, "1 :   :   1\n");
    free(result);

    // Test with invalid expression (should handle it safely)
    result = generate_postfix_row(0, 2, "", 0);
    CU_ASSERT_PTR_NULL(result);
}

void test_generate_postfix_truth_table_segment(void)
{
    char *result;
    // Test a segment from the beginning to the first 2 rows for a simple expression
    result = generate_postfix_truth_table_segment("ab&", 0, 2);
    CU_ASSERT_STRING_EQUAL(result, "0 0 :   0 :   0\n0 1 :   0 :   0\n");
    free(result);

    // Test a different segment
    result = generate_postfix_truth_table_segment("ab|", 2, 4);
    CU_ASSERT_STRING_EQUAL(result, "1 0 :   1 :   1\n1 1 :   1 :   1\n");
    free(result);

    result = generate_postfix_truth_table_segment("", -1, 3);
    CU_ASSERT_PTR_NULL(result);

    result = generate_postfix_truth_table_segment("", 0, 1);
    CU_ASSERT_PTR_NULL(result);
}

void test_generate_true_postfix_truth_table_segment(void)
{
    char *result;
    // Test for rows that produce true results for "ab&"
    result = generate_true_postfix_truth_table_segment("ab&", 0, 4);
    CU_ASSERT_STRING_EQUAL(result, "1 1 :   1 :   1\n");
    free(result);

    // Test for "ab|"
    result = generate_true_postfix_truth_table_segment("ab|", 0, 4);
    CU_ASSERT_STRING_EQUAL(result, "0 1 :   1 :   1\n1 0 :   1 :   1\n1 1 :   1 :   1\n");
    free(result);

    result = generate_true_postfix_truth_table_segment("", -1, 3);
    CU_ASSERT_PTR_NULL(result);

    result = generate_true_postfix_truth_table_segment("", 0, 1);
    CU_ASSERT_PTR_NULL(result);
}

void test_generate_infix_row(void)
{
    char *result;
    int *expression_map = infix_map("a&b"); // Example infix map for "ab&"


    // Test with a simple expression with two variables
    result = generate_infix_row(0, 2, "ab&", expression_map, 3, 3);
    CU_ASSERT_STRING_EQUAL(result, "0 0 :  0  :   0\n");
    free(result);

    result = generate_infix_row(1, 2, "ab&", expression_map, 3, 3);
    CU_ASSERT_STRING_EQUAL(result, "0 1 :  0  :   0\n");
    free(result);

    result = generate_infix_row(3, 2, "ab&", expression_map, 3, 3);
    CU_ASSERT_STRING_EQUAL(result, "1 1 :  1  :   1\n");
    free(result);

    // Test with a single variable (no operator)
    free(expression_map);
    expression_map = infix_map("a");
    // Single a should be interpreted as postfix not infix
    result = generate_infix_row(1, 1, "a", expression_map, 1, 1);
    CU_ASSERT_STRING_EQUAL(result, "1 :   :   1\n");

    free(result);

    // Test with invalid expression (should handle it safely)
    result = generate_infix_row(0, 2, "", expression_map, 0, 0);
    CU_ASSERT_PTR_NULL(result);
    
    free(expression_map);
}

void test_generate_infix_truth_table_segment(void)
{
    char *result;

    // Test a segment from the beginning to the first 2 rows for a simple expression
    result = generate_infix_truth_table_segment("a&b", 0, 2);
    CU_ASSERT_STRING_EQUAL(result, "0 0 :  0  :   0\n0 1 :  0  :   0\n");
    free(result);

    // Test a different segment
    result = generate_infix_truth_table_segment("a|b", 2, 4);
    CU_ASSERT_STRING_EQUAL(result, "1 0 :  1  :   1\n1 1 :  1  :   1\n");
    free(result);
    
    result = generate_infix_truth_table_segment("", -1, 3);
    CU_ASSERT_PTR_NULL(result);

    result = generate_infix_truth_table_segment("", 0, 1);
    CU_ASSERT_PTR_NULL(result);
}

void test_generate_true_infix_truth_table_segment(void)
{
    char *result;

    // Test for rows that produce true results for "a&b"
    result = generate_true_infix_truth_table_segment("a&b", 0, 4);
    CU_ASSERT_STRING_EQUAL(result, "1 1 :  1  :   1\n");
    free(result);

    // Test for "a|b"
    result = generate_true_infix_truth_table_segment("a|b", 0, 4);
    CU_ASSERT_STRING_EQUAL(result, "0 1 :  1  :   1\n1 0 :  1  :   1\n1 1 :  1  :   1\n");
    free(result);

    result = generate_true_infix_truth_table_segment("", -1, 3);
    CU_ASSERT_PTR_NULL(result);

    result = generate_true_infix_truth_table_segment("", 0, 1);
    CU_ASSERT_PTR_NULL(result);
}

void test_count_unique_variables(void) {
    // Test 1: Basic test with unique variables
    CU_ASSERT_EQUAL(count_unique_variables("abc"), 3);
    CU_ASSERT_EQUAL(count_unique_variables("a + b + c"), 3);
    CU_ASSERT_EQUAL(count_unique_variables("x * y / z"), 3);
    
    // Test 2: Repeated variables
    CU_ASSERT_EQUAL(count_unique_variables("aa"), 1);
    CU_ASSERT_EQUAL(count_unique_variables("abacaba"), 3);
    CU_ASSERT_EQUAL(count_unique_variables("xx + yy + zz"), 3);

    // Test 3: No variables
    CU_ASSERT_EQUAL(count_unique_variables("123 + 456"), 0);
    CU_ASSERT_EQUAL(count_unique_variables("!@#$%^&*()"), 0);
    CU_ASSERT_EQUAL(count_unique_variables(""), 0);

    // Test 4: Mixed alphanumeric input
    CU_ASSERT_EQUAL(count_unique_variables("a1b2c3"), 3);
    CU_ASSERT_EQUAL(count_unique_variables("a1 + b2 - c3"), 3);

    // Test 5: Uppercase letters (should be ignored)
    CU_ASSERT_EQUAL(count_unique_variables("ABC"), 0);
    CU_ASSERT_EQUAL(count_unique_variables("aAbBcC"), 3);
}

// Main method to run the tests
int main()
{
    FILE *null_file = freopen("/dev/null", "w", stderr);
    if (null_file == NULL) {
        perror("Failed to redirect stderr");
        return 1;
    }
    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    // Add suites and test cases
    CU_pSuite suite1 = CU_add_suite("Test int_to_binary_string", 0, 0);
    CU_add_test(suite1, "Invalid int_to_binary_string input", test_int_to_binary_string_invalid_input);
    CU_add_test(suite1, "Valid int_to_binary_string input", test_int_to_binary_string_valid_input);

    CU_pSuite suite2 = CU_add_suite("Test replace_with_binary", 0, 0);
    CU_add_test(suite2, "Invalid replace_with_binary input", test_replace_with_binary_invalid_input);
    CU_add_test(suite2, "Valid replace_with_binary input", test_replace_with_binary_valid_input);

    CU_pSuite suite3 = CU_add_suite("Test convert_evaled_rpn_to_infix", 0, 0);
    CU_add_test(suite3, "Invalid convert_evaled_rpn_to_infix input", test_convert_evaled_rpn_to_infix_invalid_input);
    CU_add_test(suite3, "Valid convert_evaled_rpn_to_infix input", test_convert_evaled_rpn_to_infix_valid_input);

    CU_pSuite suite4 = CU_add_suite("Test shunting_yard", 0, 0);
    CU_add_test(suite4, "Invalid shunting_yard input", test_shunting_yard_invalid_input);
    CU_add_test(suite4, "Valid shunting_yard  input", test_shunting_yard_valid_input);

    CU_pSuite suite5 = CU_add_suite("Test infix_map", 0, 0);
    CU_add_test(suite5, "Check infix_map output", test_infix_map);

    CU_pSuite suite6 = CU_add_suite("Test int_stack", 0, 0);
    CU_add_test(suite6, "Check infix_map output", test_infix_map);

    CU_pSuite suite7 = CU_add_suite("Test Stack", 0, 0);
    CU_add_test(suite7, "Test all Stack operations", test_stack_operations);

    CU_pSuite suite8 = CU_add_suite("Test Evaluation", 0, 0);
    CU_add_test(suite8, "Test Evaluation", test_evaluate_expr);

    CU_pSuite suite9 = CU_add_suite("Test generate_postfix_row", 0, 0);
    CU_add_test(suite9, "Test Generate postfix row", test_generate_postfix_row);

    CU_pSuite suite10 = CU_add_suite("Test generate postfix segment", 0, 0);
    CU_add_test(suite10, "Test generate postfix segment", test_generate_postfix_truth_table_segment);

    CU_pSuite suite11 = CU_add_suite("Test generate postfix true segment", 0, 0);
    CU_add_test(suite11, "Test generate true postfix segment", test_generate_true_postfix_truth_table_segment);

    CU_pSuite suite12 = CU_add_suite("test generate infix row", 0, 0);
    CU_add_test(suite12, "Test generate infix row", test_generate_infix_row);

    CU_pSuite suite13 = CU_add_suite("Test generate infix segment", 0, 0);
    CU_add_test(suite13, "Test generate infix truth table segment", test_generate_infix_truth_table_segment);

    CU_pSuite suite14 = CU_add_suite("Test generate infix true segment", 0, 0);
    CU_add_test(suite14, "Test generate true infix truth table segment", test_generate_true_infix_truth_table_segment);

    CU_pSuite suite15 = CU_add_suite("Test find_nr_of_vars", 0, 0);
    CU_add_test(suite15, "Test find_nr_of_vars", test_count_unique_variables);


    // Run all tests using CUnit Basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Clean up the registry
    CU_cleanup_registry();
    return CU_get_error();
}
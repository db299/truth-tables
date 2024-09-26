#pragma once
#include <stdbool.h>

/*
 * Function to evaluate a logical rpn expression.
 * @param expression The expression being evaluated
 * @return A string consisting of all intermediate results when evaluating
 * the expression
 */
char *evaluate_expr(const char *expression);

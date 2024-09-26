#pragma once
#define MAX_EXPR_SIZE 1000

#include <stdbool.h>

/**
 * Function to convert an infix expression to a rpn expression using the shunting yard algorithm
 * @param expression The expression being converted
 * @return The expression after the conversion
*/
char *shunting_yard(const char *expression);

/**
 * Function to get the infix map of an expression - so where the operators in the infix expression would
 * map onto the rpn expression
 * @param expression The infix expression for which we are computing the map
 * @return An array of integers, with array[i] = the possition of the element of rpn[i] in infix
*/
int *infix_map(const char *expression);

/**
 * Checks if the expression passed is a valid infix expresson
 * @param expression The expression being checked
 * @return true if it is valid, false otherwise
*/
bool is_valid_infix(const char *expression);
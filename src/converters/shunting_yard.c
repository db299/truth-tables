#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "../data_structs/int_stack.h"
#include "../data_structs/stack.h"

#include "shunting_yard.h"

static int precedence(char op)
{
    switch (op)
    {
    case '-':
        return 5; // NOT
    case '&':
        return 4; // AND
    case '#':
        return 3; // XOR
    case '|':
        return 2; // OR
    case '>':
        return 1; // IMPLICATION
    case '=':
        return 0; // IFF
    default:
        return -1;
    }
}

static bool is_right_associative(char op)
{
    return (op == '-' || op == '>');
}

static bool is_operator(char token)
{
    return (token == '-' || token == '&' || token == '#' ||
            token == '|' || token == '>' || token == '=');
}

char *shunting_yard(const char *expression)
{
    if (!is_valid_infix(expression))
    {
        fprintf(stderr, "Invalid infix expression\n");
        return (char *)NULL;
    }
    Stack operator_stack;
    operator_stack.top = -1;

    char *output = malloc(strlen(expression) * sizeof(char) + 1);
    if (output == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for output expression in %s at line %d", __FILE__, __LINE__);
        return (char *)NULL;
    }

    int output_index = 0;

    for (int i = 0; expression[i] != '\0'; i++)
    {
        char token = expression[i];

        if (isspace(token))
        {
            continue; // Skip whitespace
        }
        // Operand (a-z, 0 or 1).
        else if ((islower(token)) || token == '1' || token == '0')
        {
            output[output_index++] = token;
        }
        else if (is_operator(token))
        { // Operator
            while (!is_empty(&operator_stack) && is_operator(peek(&operator_stack)) &&
                   ((is_right_associative(token) && precedence(token) < precedence(peek(&operator_stack))) ||
                    (!is_right_associative(token) && precedence(token) <= precedence(peek(&operator_stack)))))
            {

                output[output_index++] = pop(&operator_stack);
            }

            push(&operator_stack, token);
        }
        else if (token == '(')
        { // Left parenthesis
            push(&operator_stack, token);
        }
        else if (token == ')')
        { // Right parenthesis
            while (!is_empty(&operator_stack) && peek(&operator_stack) != '(')
            {
                output[output_index++] = pop(&operator_stack);
            }
            if (!is_empty(&operator_stack) && peek(&operator_stack) == '(')
            {
                pop(&operator_stack); // Discard the left parenthesis
            }
        }

        else
        {
            fprintf(stderr, "Unknwon symbol %c\n", token);
            free(output);
            return (char *)NULL;
        }
    }

    // Pop all remaining operators from the stack
    while (!is_empty(&operator_stack))
    {
        output[output_index++] = pop(&operator_stack);
    }

    output[output_index] = '\0'; // Null-terminate the output string

    return output;
}

/**
 * This implementation effectively functions as running the shunting yard algorithm but keeping track
 * of where each operator is going in the infix expression so that this process can be reversed later on.
 */
int *infix_map(const char *expression)
{
    if (!is_valid_infix(expression))
    {
        fprintf(stderr, "Invalid infix expression\n");
        return (int *)NULL;
    }
    int expr_len = strlen(expression);
    int *rpnArr = (int *)malloc(expr_len * sizeof(int));
    Stack operator_stack;
    IntStack positions_stack;
    operator_stack.top = -1;
    positions_stack.top = -1;
    int rpn_index = 0;

    for (int i = 0; expression[i] != '\0'; i++)
    {
        char token = expression[i];

        if (isspace(token))
        {
            continue; // Skip whitespace
        }

        else if (islower(token) || token == '1' || token == '0')
        { // Operand (a-z, 0-9)
            rpnArr[rpn_index++] = i;
        }
        else if (is_operator(token))
        { // Operator
            while (!is_empty(&operator_stack) && is_operator(peek(&operator_stack)) &&
                   ((is_right_associative(token) && precedence(token) < precedence(peek(&operator_stack))) ||
                    (!is_right_associative(token) && precedence(token) <= precedence(peek(&operator_stack)))))
            {

                rpnArr[rpn_index++] = int_stack_pop(&positions_stack);
                pop(&operator_stack);
            }

            push(&operator_stack, token);
            int_stack_push(&positions_stack, i);
        }
        else if (token == '(')
        { // Left parenthesis
            push(&operator_stack, token);
            int_stack_push(&positions_stack, -1); // Mark with -1 for parenthesis
        }
        else if (token == ')')
        { // Right parenthesis
            while (!is_empty(&operator_stack) && peek(&operator_stack) != '(')
            {
                rpnArr[rpn_index++] = int_stack_pop(&positions_stack);
                pop(&operator_stack);
            }
            if (!is_empty(&operator_stack) && peek(&operator_stack) == '(')
            {
                pop(&operator_stack);            // Discard left parenthesis
                int_stack_pop(&positions_stack); // Discard corresponding position
            }
        }
        else
        {
            fprintf(stderr, "Unknwon symbol %c\n", token);
            free(rpnArr);
            return (int *)NULL;
        }
    }

    // Pop remaining operators
    while (!is_empty(&operator_stack))
    {
        rpnArr[rpn_index++] = int_stack_pop(&positions_stack);
        pop(&operator_stack);
    }

    // Mark unused slots in rpnArr with -1
    for (int i = rpn_index; i < expr_len; i++)
    {
        rpnArr[i] = -1;
    }

    return rpnArr;
}

bool is_valid_infix(const char *expression)
{
    int balance = 0;               // To track the balance of parentheses
    bool expecting_operand = true; // To track expected character types
    for (int i = 0; expression[i] != '\0'; i++)
    {
        char token = expression[i];
        // Skip spaces
        if (token == ' ')
        {
            continue;
        }

        // Check for valid characters
        if (!isalpha(token) && token != '0' && token != '1' && token != '|' &&
            token != '&' && token != '#' && token != '>' && token != '=' &&
            token != '-' && token != '(' && token != ')')
        {
            return false; // Invalid character
        }

        // Handle parentheses
        if (token == '(')
        {
            balance++;
            expecting_operand = true; // After an opening parenthesis, expect an operand or a unary operator
        }
        else if (token == ')')
        {
            if (balance == 0 || expecting_operand)
            {
                return false; // Unmatched closing parenthesis or expecting operand before closing parenthesis
            }
            balance--;
            expecting_operand = false; // After a closing parenthesis, expect an operator or end of expression
        }
        else if (isalnum(token) || token == '0' || token == '1')
        {
            if (!expecting_operand)
            {
                return false; // Unexpected operand
            }
            expecting_operand = false; // After an operand, expect an operator or closing parenthesis
        }
        else if (token == '|' || token == '&' || token == '#' ||
                 token == '>' || token == '=')
        {
            if (expecting_operand)
            {
                return false; // Unexpected operator
            }
            expecting_operand = true; // After an operator, expect an operand or opening parenthesis
        }
        else if (token == '-')
        {
            if (!expecting_operand)
            {
                return false; // Unexpected unary operator (logical NOT)
            }
            // Continue expecting an operand after a unary operator
        }
        else
        {
            return false; // Any other invalid situation
        }
    }
    // Check if all parentheses are closed and the last character is valid
    return (balance == 0 && !expecting_operand);
}

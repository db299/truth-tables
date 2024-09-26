#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../data_structs/stack.h"

#include "evaluation.h"

static bool apply_operator(char operator, bool operand1, bool operand2)
{
    bool result;
    switch (operator)
    {
    case '|':
        result = operand1 || operand2;
        break;
    case '&':
        result = operand1 && operand2;
        break;
    case '#':
        result = operand1 != operand2;
        break;
    case '>':
        result = !operand1 || operand2;
        break;
    case '=':
        result = operand1 == operand2;
        break;
    case '-':
        result = !operand1;
        break;
    default:
        fprintf(stderr, "Error: Unknwon operator %c\n", operator);
        return NULL;
    }

    return result;
}

char *evaluate_expr(const char *expression)
{
    Stack stack;
    init_stack(&stack);
    int len = strlen(expression);
    char *output = (char *)malloc((len + 1) * sizeof(char)); // Allocate memory for the output string
    if (output == NULL)
    {
        fprintf(stderr, "Memory allocation failed in %s\n", __FILE__);
        return NULL;
    }

    int output_index = 0; // Track the current position in the output string

    for (int i = 0; i < len; i++)
    {
        char token = expression[i];

        if (token == ' ')
        {
            output[output_index++] = ' ';
        }
        // Evaluating for operators
        else if (token == '|' || token == '&' || token == '#' || token == '>' || token == '=')
        {
            char elem1 = pop(&stack);
            char elem2 = pop(&stack);
            if (elem1 == NULL || elem2 == NULL)
            {
                fprintf(stderr, "Error: Bad Expression in %s at line %d", __FILE__, __LINE__);
                free(output);
                return NULL;
            }
            bool operand2 = (elem2 == '1');
            bool operand1 = (elem1 == '1');

            bool result = apply_operator(token, operand1, operand2);
            output[output_index++] = result ? '1' : '0'; // Store the result directly in the output string
            push(&stack, result ? '1' : '0');
        }

        // Evaluating for NOT operator
        else if (token == '-')
        {
            char elem1 = pop(&stack);
            if (elem1 == NULL)
            {
                fprintf(stderr, "Error: Bad Expression in %s at line %d", __FILE__, __LINE__);
                free(output);
                return NULL;
            }

            bool operand = (elem1 == '1');
            bool result = apply_operator(token, operand, true);
            output[output_index++] = result ? '1' : '0';
            push(&stack, result ? '1' : '0');
        }

        // Evaluating constants
        else if (token == '1' || token == '0')
        {
            output[output_index++] = ' ';
            push(&stack, token);
        }

        // Handling unknown operators
        else
        {
            free(output); // Free allocated memory before exiting
            fprintf(stderr, "Error: Bad Expression\n");
            return NULL;
        }
    }

    if (stack.top != 0)
    {
        fprintf(stderr, "Bad expression\n");
        free(output);
        return NULL;
    }

    output[output_index] = '\0'; // Null-terminate the output string
    return output;
}

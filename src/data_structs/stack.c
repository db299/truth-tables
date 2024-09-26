#include "stack.h"

void init_stack(Stack *stack)
{
    stack->top = -1;
}

void push(Stack *stack, char value)
{
    if (stack->top < STACK_SIZE - 1)
    {
        stack->top++;
        stack->data[stack->top] = value;
    }
    else
    {
        fprintf(stderr, "Error: Stack overflow\n");
        return;
    }
}

char pop(Stack *stack)
{
    if (stack->top >= 0)
    {
        char value = stack->data[stack->top];
        stack->top--;
        return value;
    }
    else
    {
        fprintf(stderr, "Error: Stack underflow\n");
        return NULL;
    }
}

bool is_empty(Stack *stack)
{
    return stack->top == -1;
}

char peek(Stack *stack)
{
    if (stack->data[stack->top] < 0)
    {
        fprintf(stderr, "Error: Stack underflow\n");
        return NULL;
    }
    return stack->data[stack->top];
}

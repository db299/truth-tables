#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

typedef struct
{
    int top;
    int items[MAX_SIZE];
} IntStack;

void int_stack_init(IntStack *stack)
{
    stack->top = -1;
}

int int_stack_is_empty(IntStack *stack)
{
    return stack->top == -1;
}

int int_stack_is_full(IntStack *stack)
{
    return stack->top == MAX_SIZE - 1;
}

void int_stack_push(IntStack *stack, int value)
{
    if (int_stack_is_full(stack))
    {
        fprintf(stderr, "Stack overflow: Cannot push %d, stack is full\n", value);
        return;
    }
    stack->items[++(stack->top)] = value;
}

int int_stack_pop(IntStack *stack)
{
    if (int_stack_is_empty(stack))
    {
        fprintf(stderr, "Stack underflow: Cannot pop, stack is empty\n");
        return NULL;
    }
    return stack->items[(stack->top)--];
}

int int_stack_peek(IntStack *stack)
{
    if (int_stack_is_empty(stack))
    {
        fprintf(stderr, "Stack is empty: Cannot peek\n");
        return NULL;
    }
    return stack->items[stack->top];
}

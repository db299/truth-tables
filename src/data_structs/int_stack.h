#pragma once

#define MAX_SIZE 1000 // Define maximum size of the stack

// Stack structure for managing integers
typedef struct
{
    int top;             
    int items[MAX_SIZE]; 
} IntStack;

// Function to initialize the stack
void int_stack_init(IntStack *stack);

// Function to check if the stack is empty
int int_stack_is_empty(IntStack *stack);

// Function to check if the stack is full
int int_stack_is_full(IntStack *stack);

// Function to push an integer onto the stack
void int_stack_push(IntStack *stack, int value);

// Function to pop an integer from the stack
int int_stack_pop(IntStack *stack);

// Function to peek at the top integer of the stack without removing it
int int_stack_peek(IntStack *stack);

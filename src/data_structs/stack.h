#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STACK_SIZE 1000

// Structure representing a stack for char values
typedef struct
{
    char data[STACK_SIZE];
    int top;
} Stack;

// Function to push a char value onto the stack
void push(Stack *stack, char value);

// Function to pop a char value from the stack
char pop(Stack *stack);

// Function to initialise the stack
void init_stack(Stack *stack);

// Function to peek the top element
char peek(Stack *stack);

// Function to check if the stack is empty
bool is_empty(Stack *stack);
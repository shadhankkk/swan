#ifndef STACK_H
#define STACK_H
#include "AST.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct STACK_STRUCT
{
    AST_T** stack;
    size_t size;
} stack_T;

stack_T* init_stack();

void stack_push_back(stack_T* s, AST_T* node);

AST_T* stack_get_back(stack_T* s);

void stack_pop_back(stack_T* s);

bool is_empty_stack(stack_T* stack);


#endif


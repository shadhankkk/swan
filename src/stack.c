#include "include/stack.h"
#include <stdio.h>

stack_T* init_stack()
{
    stack_T* s = calloc(1, sizeof(struct STACK_STRUCT));
    s->stack = (void*) 0;
    s->size = 0;

    return s;
}

void stack_push_back(stack_T* s, AST_T* node)
{
    ++s->size;
    if(s->stack == (void*) 0)
    {
        s->stack = calloc(1, sizeof(struct AST_STRUCT*));
        s->stack[0] = node;
    }
    else
    {
        s->stack = realloc(
            s->stack,
            s->size * sizeof(struct AST_STRUCT*)
        );
        s->stack[s->size - 1] = node;
    }

    return;
}

AST_T* stack_get_back(stack_T* s)
{
    if(is_empty_stack(s))
    {
        printf("Cannot pop empty stack!\n");
        exit(1);
    }

    AST_T* res = s->stack[s->size - 1];
    return res;
}

void stack_pop_back(stack_T* s)
{
    AST_T** temp = s->stack + (s->size - 1);
    --s->size;

    if(s->size == 0)
    {
        s->stack = (void*) 0;
    }
    else
    {
        //free(temp);
    }
}

bool is_empty_stack(stack_T* s)
{
    return s->size == 0;
}
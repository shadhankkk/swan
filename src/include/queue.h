#ifndef QUEUE_H
#define QUEUE_H
#include "AST.h"
#include <stdbool.h>


typedef struct QUEUE_STRUCT
{
    AST_T** queue;
    size_t size;
} queue_T;

queue_T* init_queue();

void enqueue(queue_T* queue, AST_T* node);

AST_T* dequeue(queue_T* queue);

bool is_empty_queue(queue_T* queue);

#endif
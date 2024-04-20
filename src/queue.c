#include "include/queue.h"

queue_T* init_queue()
{
    queue_T* q = calloc(1, sizeof(struct QUEUE_STRUCT));
    q->queue = (void*) 0;
    q->size = 0;

    return q;
}

void enqueue(queue_T* q, AST_T* node)
{   
    ++q->size;
    if(q->queue == (void*) 0)
    {
        q->queue = calloc(1, sizeof(struct AST_STRUCT*));
        q->queue[0] = node;
    }
    else
    {
        q->queue = realloc(
            q->queue,
            q->size * sizeof(struct AST_STRUCT*)
        );
        q->queue[q->size - 1] = node;
    }

    return;
}

AST_T* dequeue(queue_T* q)
{
    AST_T** temp = q->queue;
    AST_T* res = q->queue[0];
    --q->size;
    if(q->size == 0)
    {
        q->queue = (void*) 0;
    }
    else
    {
        ++q->queue;
        //free(temp);
    }

    return res;
}

bool is_empty_queue(queue_T* q)
{
    return q->queue == (void*) 0;
}
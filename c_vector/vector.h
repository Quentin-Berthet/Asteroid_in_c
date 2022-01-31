#ifndef VECTOR_H
#define VECTOR_H

#include <pthread.h>

#define VECTOR_INIT_CAPACITY 4

typedef struct vector {
    void **content; // actual content of the vector
    int capacity;   // capacity allocated
    int length;     // actual length
    pthread_mutex_t *mutex;
} vector;

void vector_init(vector *v);
int vector_length(vector *v);
void vector_push(vector *v, void *element);           // push an element
void vector_set(vector *v, int index, void *element); // set index-th element
void *vector_get(vector *v, int index);               // return index-th element
void *vector_remove(vector *v,
                    int index); // remove index-th element and return it
void vector_drain_into(vector *v,
                       vector *rhs); // empties vector v into rhs (v is in state
// empty, but not freed)
void vector_insert(vector *v, int index); // insert at index-th element
void vector_empty(vector *v);
void vector_free(vector *v);

#endif


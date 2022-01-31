#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

static void vector_resize(vector *v, int capacity);

void vector_init(vector *v) {
    v->capacity = VECTOR_INIT_CAPACITY;
    v->length = 0;
    v->content = calloc(v->capacity, sizeof(void *));
    v->mutex = calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(v->mutex, NULL);
}

int vector_length(vector *v) {
    pthread_mutex_lock(v->mutex);
    int length = v->length;
    pthread_mutex_unlock(v->mutex);
    return length;
}

static void vector_resize(vector *v, int capacity) {
#ifdef DEBUG_ON
    printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif
    void **content = realloc(v->content, sizeof(void *) * capacity);
    if (content) {
        v->content = content;
        v->capacity = capacity;
    }
}

void vector_push(vector *v, void *element) {
    pthread_mutex_lock(v->mutex);
    if (v->capacity == v->length)
        vector_resize(v, v->capacity * 2);
    v->content[v->length] = element;
    v->length += 1;
    pthread_mutex_unlock(v->mutex);
}

void vector_set(vector *v, int index, void *element) {
    pthread_mutex_lock(v->mutex);
    if (index >= 0 && index < v->length)
        v->content[index] = element;
    pthread_mutex_unlock(v->mutex);
}

void *vector_get(vector *v, int index) {
    pthread_mutex_lock(v->mutex);
    if (index >= 0 && index < v->length) {
        void *element = v->content[index];
        pthread_mutex_unlock(v->mutex);
        return element;
    }
    pthread_mutex_unlock(v->mutex);
    return NULL;
}

void *vector_remove(vector *v, int index) {
    pthread_mutex_lock(v->mutex);
    if (index < 0 || index >= v->length) {
        pthread_mutex_unlock(v->mutex);
        return NULL;
    }

    void *elem = v->content[index];

    for (int i = index; i < v->length - 1; i++) {
        v->content[i] = v->content[i + 1];
        v->content[i + 1] = NULL;
    }

    v->length--;

    if (v->length > 0 && v->length == v->capacity / 4)
        vector_resize(v, v->capacity / 2);

    pthread_mutex_unlock(v->mutex);
    return elem;
}

void vector_drain_into(vector *v, vector *rhs) {
    for (int i = 0; i < vector_length(v); ++i) {
        vector_push(rhs, vector_get(v, i));
        vector_set(v, i, NULL); // sets all elems of v to NULL
    }
    vector_empty(v);
}

void vector_empty(vector *v) {
    vector_free(v);
    vector_init(v);
}

void vector_free(vector *v) {
    pthread_mutex_lock(v->mutex);
    for (int i = 0; i < v->length; ++i) {
        free(v->content[i]);
        v->content[i] = NULL;
    }
    free(v->content);
    v->content = NULL;
    v->length = 0;
    v->capacity = 0;
    pthread_mutex_unlock(v->mutex);
    pthread_mutex_destroy(v->mutex);
    free(v->mutex);
    v->mutex = NULL;
}

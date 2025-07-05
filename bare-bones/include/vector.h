#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdbool.h>
#include "mem.h"

#define VECTOR_INIT_CAPACITY 64
#define SUCCESS 0
#define UNDEFINED -1

typedef struct {
    void** items;
    int capacity;
    int size;
} List;

typedef struct Vector vector;

struct Vector {
    List vectorList;
    int (*size)(vector*);
    int (*resize)(vector*, int);
    int (*add)(vector*, void*);
    int (*set)(vector*, int, void*);
    void* (*get)(vector*, int);
    int (*delete)(vector*, int);
    int (*free)(vector*);
};

int _size(vector* v) {
    if(!v) return UNDEFINED;
    
    return v->vectorList.size;
}

int _resize(vector* v, int capacity) {
    if(!v || capacity <= 0) return UNDEFINED;

    void **items = realloc(v->vectorList.items, sizeof(void *) * capacity);
    if(!items) return UNDEFINED;

    v->vectorList.items = items;
    v->vectorList.capacity = capacity;

    return SUCCESS;
}

int _add(vector* v, void* item) {
	if(!v) return UNDEFINED;

	if(v->vectorList.size == v ->vectorList.capacity)
		if(_resize(v, v->vectorList.capacity * 2) == UNDEFINED) return UNDEFINED;

	v->vectorList.items[v->vectorList.size++] = item;

	return SUCCESS;
}

int _set(vector* v, int index, void* item) {
	if(!v) return UNDEFINED;
	if(!((index >= 0) && (index < v->vectorList.size))) return UNDEFINED;

	v->vectorList.items[index] = item;

	return SUCCESS;
}

void* _get(vector* v, int index) {
	if(!v) return NULL;
	if(!((index >= 0) && (index < v->vectorList.size))) return NULL;

	return v->vectorList.items[index];
}

int _delete(vector* v, int index) {
    if(!v) return UNDEFINED;
    if((index < 0) || (index >= v->vectorList.size)) return UNDEFINED;
    
    v->vectorList.items[index] = NULL;
    for(int i = index; i < v->vectorList.size - 1; ++i) {
        v->vectorList.items[i] = v->vectorList.items[i+1];
        v->vectorList.items[i+1] = NULL;
    }

    v->vectorList.size--;
    
    bool needs_shrink = (v->vectorList.size > 0) && (v->vectorList.size == v->vectorList.capacity/4);
    if(!needs_shrink) return SUCCESS;
    
    return _resize(v, v->vectorList.capacity/2);
}

void vector_init(vector* v) {
    if(!v) return;

   	v->vectorList.items = malloc(VECTOR_INIT_CAPACITY * sizeof(void*));
    v->vectorList.capacity = VECTOR_INIT_CAPACITY;
    v->vectorList.size = 0;
    
    v->size = _size;
    v->resize = _resize;
    v->add = _add;
    v->set = _set;
    v->get = _get;
    v->delete = _delete;
    //v->free = _free;
}

#endif // VECTOR_H

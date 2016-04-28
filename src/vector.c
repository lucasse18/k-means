#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

inline void vector_init(vector *v) {
  // initialize size and capacity
  v->size = 0;
  v->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->vec
  v->vec = malloc(sizeof(double) * v->capacity);
}

inline void vector_resize(vector *v, size_t new_cap) {
  v->capacity = new_cap;
  v->vec = realloc(v->vec, sizeof(double) * v->capacity);
}

inline  void vector_append(vector *v, double value) {
  // make sure there's room to expand into
  if (v->size >= v->capacity)
    vector_resize(v, (size_t)(v->capacity * RESIZE_FACTOR));

  // append the value and increment vector->size
  v->vec[v->size++] = value;
}

inline void vector_set(vector *v, int index, double value) {
  if (index >= 0 && index < v->size)
    v->vec[index] = value;
}

inline double vector_get(vector *v, int index) {
  if (index >= v->size || index < 0) {
    printf("Index %d out of bounds.", index);
    return 0;
  }
  return v->vec[index];
}

inline size_t vector_size(vector *v) {
  return v->size;
}

inline void vector_delete(vector *v, int index) {
  if (index >= v->size || index < 0) {
    printf("Index %d out of bounds.", index);
    return;
  }

  //decrement v->size
  v->size--;

  //check to see if there is too much space available
  if (v->size < (v->capacity) / RESIZE_FACTOR)
    vector_resize(v, (size_t) (v->capacity / RESIZE_FACTOR));//FIXME rounded up?
}

inline void vector_free(vector *v) {
  free(v->vec);
}

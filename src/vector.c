#include <stdio.h>
#include <stdlib.h>

#include "../include/vector.h"

inline void vector_init(Vector *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->vec
  vector->vec = malloc(sizeof(double) * vector->capacity);
}

inline void vector_resize(Vector *vector, int new_cap) {
  vector->capacity = new_cap;
  vector->vec = realloc(vector->vec, sizeof(double) * vector->capacity);
}

inline  void vector_append(Vector *vector, double value) {
  // make sure there's room to expand into
  if (vector->size >= vector->capacity)
    vector_resize(vector, (int) (vector->capacity * RESIZE_FACTOR));

  // append the value and increment vector->size
  vector->vec[vector->size++] = value;
}

inline void vector_set(Vector *vector, int index, double value) {
  if (index >= 0 && index < vector->size)
    vector->vec[index] = value;
}

inline double vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds.", index);
    return 0;
  }
  return vector->vec[index];
}

inline int vector_size(Vector *vector) {
  return vector->size;
}

inline void vector_delete(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds.", index);
    return;
  }

  //decrement vector->size
  vector->size--;

  //check to see if there is too much space available
  if (vector->size < (vector->capacity) / RESIZE_FACTOR)
    vector_resize(vector, (int) (vector->capacity / RESIZE_FACTOR));//FIXME rounded up?
}

inline void vector_free(Vector *vector) {
  free(vector->vec);
}

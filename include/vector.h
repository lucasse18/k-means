#ifndef VECTOR_H
#define VECTOR_H


//TODO optimize values for cache
#define VECTOR_INITIAL_CAPACITY 100
#define RESIZE_FACTOR 1.5

//convenience macros
#define VECTOR_INIT(vec) vector_init(&vec)
#define VECTOR_APPEND(vec, value) vector_append(&vec, value)
#define VECTOR_SET(vec, index, value) vector_set(&vec, index, value)
#define VECTOR_GET(vec, index) vector_get(&vec, index)

#define VECTOR_DELETE(vec, index) vector_delete(&vec, index)

#define VECTOR_SIZE(vec) vector_size(&vec)
#define VECTOR_FREE(vec) vector_free(&vec)

typedef struct {
  int size;      //slots used so far
  int capacity;  //total available slots
  double *vec;   //array we're storing
} Vector;

void vector_init(Vector *vector);
void vector_resize(Vector *vector, int new_cap);
void vector_append(Vector *vector, double value);
void vector_set(Vector *vector, int index, double value);
double vector_get(Vector *vector, int index);

void vector_delete(Vector *vector, int index); //TODO

int vector_size(Vector *vector);
void vector_free(Vector *vector);


#endif

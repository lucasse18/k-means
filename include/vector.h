#ifndef VECTOR_H
#define VECTOR_H


//TODO optimize values for cache sizes
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
  size_t size;      //slots used so far
  size_t capacity;  //total available slots
  double *vec;   //array we're storing
} vector;

void vector_init(vector *v);
void vector_resize(vector *v, size_t new_cap);
void vector_append(vector *v, double value);
void vector_set(vector *v, int index, double value);
double vector_get(vector *v, int index);
void vector_delete(vector *v, int index);
size_t vector_size(vector *v);
void vector_free(vector *v);


#endif //VECTOR_H

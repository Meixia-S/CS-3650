/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector 
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be 
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {
 vect_t *v = (vect_t*) malloc(sizeof(vect_t));
 assert(v != NULL);
 v->data = malloc(sizeof(char*) * VECT_INITIAL_CAPACITY);
 assert(v->data != NULL);
 v->size = 0;
 v->capacity = VECT_INITIAL_CAPACITY;

 return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {

  for (int i = 0; i < v->size; i ++) {
    free(v->data[i]);
  }
  
  free(v->data);
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);
  
  return v->data[idx];
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. (so we do not have to?) */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  char *copy = malloc(sizeof(char) * (strlen(v->data[idx]) + 1));
  assert(copy != NULL);
  strcpy(copy, v->data[idx]);
  return copy;
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);
  
  char *copy = malloc(sizeof(char) * (strlen(elt) + 1));
  assert(copy != NULL);
  strcpy(copy, elt);
  free(v->data[idx]);
  v->data[idx] = copy;
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);

  if(v->size == v->capacity) {
    v->capacity =  VECT_GROWTH_FACTOR * v->size;
    v->data = realloc(v->data, (sizeof(char*) *  v->capacity));
    assert(v->data != NULL);
  } 
 
  char *copy = malloc(sizeof(char) * (strlen(elt) + 1));
  assert(copy != NULL);
  strcpy(copy, elt);
  v->data[v->size] = copy;
  v->size = v->size + 1;
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);
  assert(v->size > 0);
  
  v->size = v->size - 1;
  free(v->data[v->size]);
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);
  
  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);
  
  return v->capacity;
}

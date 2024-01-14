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
#include <stdio.h>

#include "vector.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {

  /* [TODO] Complete the function */

  vect_t *v = (vect_t*)malloc(sizeof(vect_t));
  v->size = 0;
  v->capacity = VECT_INITIAL_CAPACITY;
  v->data= (char**)malloc(v->capacity * sizeof(char*));
  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {

  /* [TODO] Complete the function */
	if(v->size != 0 ){
		for(int i =0; i < v->size; i++){
		      free(v->data[i]);}}	
	free(v->data);
	free(v);

}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  /* [TODO] Complete the function */
  /* re*turn a const pointer to the vector's copy of the string*/
 
  const char* copied_string = v->data[idx]; 
  return copied_string ;
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  /* [TODO] Complete the function */
  /* return a copy of the string */
  char *string = v->data[idx];
  char *copied_string = (char *)malloc((strlen(string)+1)* sizeof(char));
  strcpy(copied_string, string);


  return copied_string;
}
/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);
  /* [TODO] Complete the function */
  free(v->data[idx]);

  v->data[idx] = (char*) malloc(sizeof(char)* (strlen(elt) + 1));
  strcpy(v->data[idx], elt); 
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);
  /* [TODO] Complete the function */
  /*if the size reach the capacity, increase the size first*/
  if(v->size == v->capacity){
	   v->capacity = v->capacity * VECT_GROWTH_FACTOR;
            v->data= (char**)realloc(v->data, v->capacity * sizeof(char*));
          
  }
 
	   v->data[v->size] = malloc(strlen(elt)+1);
            strcpy(v->data[v->size], elt);
             v->size++;

 
}
/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);

  /* [TODO] Complete the function */
  free(v->data[v->size-1]);
  v->size--; 

}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);

  /* [TODO] Complete the function */

  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);

  /* [TODO] Complete the function */

  return v->capacity;
}

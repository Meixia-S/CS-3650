// Basic test

#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <assert.h>

#define ARRAY_ELEMENTS 1024

int main() {
  fprintf(stderr, 
      "=======================================================================\n"
      "This test should only call sbrk once to request %lu bytes for the\n"
      "memory returned to the user + however many bytes you need for your\n"
      "block struct. You should not get any memory error or assertion error.\n"
      "=======================================================================\n",
      ARRAY_ELEMENTS * sizeof(int));

  // Allocate some data
  printf("print statment 1\n");
  int *data = (int *) malloc(ARRAY_ELEMENTS * sizeof(int));
   printf("print statment 2\n");

  int *old_ptr = data;

  // Do something with the data
  int i = 0;
  for (i = 0; i < ARRAY_ELEMENTS; i++) {
    data[i] = i;
  }

  // Free the data
  free(data);
  printf("print statment 5\n");
  data = NULL;

  // allocate again - we should get the same block
  printf("print statment 6\n");
  data = (int *) malloc(ARRAY_ELEMENTS * sizeof(int));
printf("print statment 7\n");
  assert(data == old_ptr);
  printf("print statment 8\n");
  old_ptr = data;

  free(data);
  data = NULL;

  // allocate a smaller chung - we should still get the same block
  data = (int *) malloc(sizeof(int));

  assert(data == old_ptr);

  free(data);

  return 0;
}

#define _DEFAULT_SOURCE
#define _BSD_SOURCE 
#include <malloc.h> 
#include <debug.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>

typedef struct block {
  size_t size;        // How many bytes beyond this metadata have been allocated for this block
  struct block *next; // The next block in the free list
  struct block *prev; // The previous block in the free list
  int isFree;         // 0 yes, 1 no   
} block_t;

// Global variables 
block_t* FIRSTBLOCK = NULL;
block_t* LASTBLOCK = NULL;

// Initializing the first block of the heap
block_t *initializingFirstBlock(size_t size) {
  int sizeOfBlock = sizeof(block_t) + size;
  block_t *firstBlock = sbrk(sizeOfBlock); 
   
  firstBlock->size = size;
  firstBlock->next = NULL;
  firstBlock->prev = NULL;
  firstBlock->isFree = 1;
  
  FIRSTBLOCK = firstBlock;
  LASTBLOCK = firstBlock;
  return firstBlock;
}

// Adding another block of memory to the heap
block_t* addingAnotherBlock(size_t size) {
  int sizeOfBlock = sizeof(block_t) + size;

  block_t *newAddedBlock = sbrk(sizeOfBlock); 
  newAddedBlock->size = size;
  newAddedBlock->next = NULL;
  newAddedBlock->prev = LASTBLOCK;
  newAddedBlock->isFree = 1;
  
  LASTBLOCK->next = newAddedBlock;
  LASTBLOCK = newAddedBlock;
  return newAddedBlock;
}

// Finding a suitable memory space for the given memory size
block_t* findSuitableSpace(size_t size) { 
  block_t *currBlock = FIRSTBLOCK;
  
  while(currBlock != NULL) {
    if(currBlock->size >= size && currBlock->isFree == 0) {
      currBlock->isFree = 1;
      return currBlock;
    } else {
      currBlock = currBlock->next;
    }
  }
  currBlock = NULL;
  return currBlock;
}

//  Allocating the given data
void *mymalloc(size_t s) {
  block_t *ptr;
  if(FIRSTBLOCK == NULL) {
    ptr = initializingFirstBlock(s);
  } else {
    ptr = findSuitableSpace(s);
    if (!ptr) {
       ptr = addingAnotherBlock(s);
    }
  } 
  debug_printf("malloc %zu bytes\n", s);
  return (void*)(ptr + 1);
}

// Allocating the given data and setting its value
void *mycalloc(size_t nmemb, size_t s) {
  size_t total = nmemb * s;
  void *ptr;
  if(FIRSTBLOCK == NULL) {
    ptr = (initializingFirstBlock(total) + 1);
  } else {
    ptr = findSuitableSpace(total);
    if (!ptr) {
       ptr = (addingAnotherBlock(total) + 1);
    }
  }
  memset(ptr, 0, total);
  debug_printf("calloc %zu bytes\n", s);
  return ptr;
}

// Free the data at the given pointer
void myfree(void *ptr) {
  block_t* currBlock;
  currBlock = (block_t*)(ptr - sizeof(block_t));
  currBlock->isFree = 0;
  debug_printf("Freed %zu memory\n", currBlock->size);
}

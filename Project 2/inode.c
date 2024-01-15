#include <stdio.h>
#include "inode.h"
#include "bitmap.h"

const int NUMBER_INODES = 256;
const int DIRECT_BLOCK_NUMS = 12;

void print_inode(inode_t *node) {
  printf("inum: %d\n refs: %d\n mode: %d\n size: %d\n block pointer: %d\n", node->inum, node->refs, node->mode, node->size, node->direct[0]);
}

inode_t *get_inode(int inum) {
   void* first_inode = 32 + get_inode_bitmap();
   inode_t* inode = (inode_t*)(first_inode + inum * sizeof(inode_t));
   return inode;
}

int alloc_inode() {
  int inum;
  for(int i = 0; i < NUMBER_INODES; i ++) {
    if(bitmap_get(get_inode_bitmap(), i) == 0) {
      bitmap_put(get_inode_bitmap(), i, 1);
      inum = i;
      break;
    }
  }
  printf("in alloc_inode: returning inum %d\n", inum);
  return inum;
}

void free_inode(int inum) {
  inode_t* inode = get_inode(inum);
  void* bm = get_inode_bitmap();
  shrink_inode(inode, 0);
  free_block(inode->direct[0]);
  bitmap_put(bm, inum, 0);
}

// indirect pointer, we get a extra data block and fill it with ints (block numbers). 

int grow_inode(inode_t *node, int size) {
  //calculate total number of blocks the size will take up
  int total_block_num = size / 4096;
  int current_inode_bnum = (node->size/ 4096) + 1; // plus 1 because of math behavior in c
  //start allocating the blocks.  start from direct block range( i <12)
  for (int i = current_inode_bnum ; i < total_block_num; i ++){
    if (i< DIRECT_BLOCK_NUMS){
      node->direct[i] = alloc_block();
    } else {
        // init the indirect block if it hasn't
        if (node->indirect == 0) { 
          node->indirect = alloc_block();
        }
        int* indirect_map = blocks_get_block(node->indirect);
        int index =  i - DIRECT_BLOCK_NUMS;
        indirect_map[index] = alloc_block();
    }
  }
   node->size = size;
    return 0;
    
}

int shrink_inode(inode_t *node, int size) {
  //calculate total number of blocks the size will take up
  int total_block_num = size / 4096;
  int current_inode_bnum_index = node->size/ 4096 ;
  //start free the block. starting from indirect
  for (int i = current_inode_bnum_index; i > total_block_num; i --){
    if (i > DIRECT_BLOCK_NUMS ){
       int* indirect_map = blocks_get_block(node->indirect);
            free_block(indirect_map[i - 12]);
            indirect_map[i - 12] = 0;
    } else if ( i = DIRECT_BLOCK_NUMS){
       int* indirect_map = blocks_get_block(node->indirect);
        free_block(indirect_map[0]); // free the actual data block
         free_block(node->indirect); // free the indirect block 
         node->indirect = 0; // flip indirect to 0
    } else {// in direct block range
        free_block(node->direct[i]);
        node->direct[i] = 0;
    }
  }
    node->size = size;
    return 0;
}

int inode_get_bnum(inode_t *node, int file_bytenum) {
   int block_num_index = file_bytenum / 4096;
    printf("in inode_get_bnum: readding the %d index of blocl\n",block_num_index);
    if (block_num_index < DIRECT_BLOCK_NUMS) {
        return node->direct[block_num_index];
    } else {
        int* indirect = blocks_get_block(node->indirect);
        return indirect[block_num_index - 12];
    }
}

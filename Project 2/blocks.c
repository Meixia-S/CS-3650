/**
 * @file blocks.c
 * @author CS3650 staff
 *
 * Implementatino of a block-based abstraction over a disk image file.
 */
#define _GNU_SOURCE
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "bitmap.h"
#include "blocks.h"
#include "inode.h"
#include "slist.h"
#include "directory.h"

const int BLOCK_COUNT = 256; // we split the "disk" into 256 blocks
const int BLOCK_SIZE = 4096; // = 4K
const int NUFS_SIZE = BLOCK_SIZE * BLOCK_COUNT; // = 1MB
const int ROOT_INODE_NUM = 0;

const int BLOCK_BITMAP_SIZE = BLOCK_COUNT / 8;
// Note: assumes block count is divisible by 8

static int min(int a, int b) {
    return a > b ? b : a;
}

static int blocks_fd = -1;
static void *blocks_base = 0;

void init_file_system(const char *image_path) {
  // setting up the two bitmaps in our file system
   blocks_init(image_path);
  
   // num of blocks for inode  = 3, the 4th blocks will be the block for root directory
    //check if blocks for inode have been initilzied. If clock bitmap slot at index 1 == 0, allocate blocks for inode struct
    if(bitmap_get(get_blocks_bitmap(), 1) == 0){
        for(int i = 0; i < 3; i ++){
            alloc_block();
        }
    }
    
    //check if there is a root direct exist in the systm(i.e if block bitmapslot at index 4 !=0). if it == 0, allocate blocks for root directory
    if(bitmap_get(get_blocks_bitmap(), 4) == 0){
          inode_t* root_inode = get_inode(ROOT_INODE_NUM);//pointer to root inode
          root_inode->inum = ROOT_INODE_NUM;
          root_inode->refs = 1;
          root_inode->mode = 040755;
          root_inode->size = 0;
          root_inode->direct[0] = alloc_block();
          root_inode->indirect = 0; 
          bitmap_put(get_inode_bitmap(), ROOT_INODE_NUM, 1);//flil inode bitmap for root_inode
     }
}

//make a filesystem object such as file
int block_mknod(const char *path, int mode){
  //make a inode for the file
  printf("start of block mknod\n");
  int new_inum = alloc_inode();
  printf("alloc new inode inum\n");
  inode_t* root_node = get_inode(ROOT_INODE_NUM);
  inode_t* new_node = get_inode(new_inum);
  new_node->inum = new_inum;
  new_node->refs = 1;
  new_node->mode = mode;
  new_node->size = 0;
  new_node->direct[0] = alloc_block();
  new_node->indirect = 0;
  printf("In block_mknod: take in mode  as %d\n", mode);
    
  //put this file into the bloc of root directory
  char* filename = (char*)malloc(sizeof(path) -1);
  for (int i = 1; i < sizeof(path); i++){
      filename[i-1] = path[i];
  }
  if(sizeof(filename) > 10) {
   return -1; 
  }
printf("in block_mknod: pass in new inum as %d, file name as %s\n",new_inum, filename);
  return directory_put(root_node, filename, new_inum);
}

//rename a file from from to to (Not Needed As nufs.c Has Functionaility There
int block_rename(const char *from, const char * to){
}

// Get the number of blocks needed to store the given number of bytes.
int bytes_to_blocks(int bytes) {
  int quo = bytes / BLOCK_SIZE;
  int rem = bytes % BLOCK_SIZE;
  if (rem == 0) {
    return quo;
  } else {
    return quo + 1;
  }
}

// Load and initialize the given disk image.
void blocks_init(const char *image_path) {
  blocks_fd = open(image_path, O_CREAT | O_RDWR, 0644);
  assert(blocks_fd != -1);

  // make sure the disk image is exactly 1MB
  int rv = ftruncate(blocks_fd, NUFS_SIZE);
  assert(rv == 0);

  // map the image to memory
  blocks_base =
      mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, blocks_fd, 0);
  assert(blocks_base != MAP_FAILED);

  // block 0 stores the block bitmap and the inode bitmap
  void *bbm = get_blocks_bitmap();
  bitmap_put(bbm, 0, 1);
}

// Close the disk image.
void blocks_free() {
  int rv = munmap(blocks_base, NUFS_SIZE);
  assert(rv == 0);
}

// Get the given block, returning a pointer to its start.
void *blocks_get_block(int bnum) { return blocks_base + BLOCK_SIZE * bnum; }

// Return a pointer to the beginning of the block bitmap.
// The size is BLOCK_BITMAP_SIZE bytes.
void *get_blocks_bitmap() { return blocks_get_block(0); }

// Return a pointer to the beginning of the inode table bitmap.
void *get_inode_bitmap() {
  uint8_t *block = blocks_get_block(0);

  // The inode bitmap is stored immediately after the block bitmap
  return (void *) (block + BLOCK_BITMAP_SIZE);
}

// Allocate a new block and return its index.
int alloc_block() {
  void *bbm = get_blocks_bitmap();

  for (int ii = 1; ii < BLOCK_COUNT; ++ii) {
    if (!bitmap_get(bbm, ii)) {
      bitmap_put(bbm, ii, 1);
      printf("+ alloc_block() -> %d\n", ii);
      return ii;
    }
  }

  return -1;
}

// Deallocate the block with the given index.
void free_block(int bnum) {
  printf("+ free_block(%d)\n", bnum);
  void *bbm = get_blocks_bitmap();
  bitmap_put(bbm, bnum, 0);
}

// Updates and shrinks/grows the inode size   
int inode_size_adjustor(const char *path, off_t size) {
  inode_t* root_dir = get_inode(0);
  int inum = directory_lookup(root_dir, path);
  
    //if file not exist
      if (inum < 0){
       return -ENOENT;
      }
  
    inode_t* file_inode = get_inode(inum);
  
  if(file_inode->size < size) {
    grow_inode(file_inode, (size - file_inode->size));
  } else if (file_inode->size > size) {
    shrink_inode(file_inode, (file_inode->size - size));
  }
  return 0;
}  
  
// Actually doing the reading of the file 
int block_read(const char *path, const char *buf, size_t size, off_t offset) {
  int rv = -1;
  inode_t* root_dir = get_inode(0);
  dirent_t* root_block = blocks_get_block(root_dir->direct[0]);
  int num_of_entries = root_dir->size / sizeof(dirent_t);
  slist_t* file_name = s_explode(path, '/');
  inode_t* file_inode;
  
  for(int i = 0; i < num_of_entries; i ++) {
    dirent_t current_dir = root_block[i];
    if(strcmp(file_name->data, current_dir.name)) {
         printf("reading the %d inode as filenode/n", current_dir.inum);
      file_inode = get_inode(current_dir.inum); 
      break;
    }
  }
    
  int buf_index = 0;
  int total_size = size;
  int ot = offset;
  while (total_size > 0) {
     char* read_buf = blocks_get_block(inode_get_bnum(file_inode, ot));
     read_buf += ot % BLOCK_SIZE;
     int read_size = min(total_size, BLOCK_SIZE - (ot % BLOCK_SIZE));
     memcpy(buf + buf_index, read_buf, read_size);
     buf_index += read_size;
     ot += read_size;
     total_size -= read_size;
  }
  
  rv = size;
  return rv;
}
 
// Actually doing the writing of the file 
int block_write(const char *path, const char *buf, size_t size, off_t offset) {
  int rv = -1;
  inode_t* root_dir = get_inode(0);
  dirent_t* root_block = blocks_get_block(root_dir->direct[0]);
  int num_of_entries = root_dir->size / sizeof(dirent_t);
  slist_t* file_name = s_explode(path, '/');
  inode_t* file_inode;
  
  for(int i = 0; i < num_of_entries; i ++) {
    dirent_t current_dir = root_block[i];
    if(strcmp(file_name->data, current_dir.name)) {
        printf("writing the %d inode as filenode/n", current_dir.inum);
      file_inode = get_inode(current_dir.inum); 
      break;
    }
  }
   if (file_inode->size != size + offset) {
        inode_size_adjustor(path, size + offset);}
  int buf_index = 0;
  int total_size = size;
  int ot = offset;
  while (total_size > 0) {
      char* read_buf = blocks_get_block(inode_get_bnum(file_inode, ot));
      read_buf += ot % BLOCK_SIZE;
      int read_size = min(total_size, BLOCK_SIZE - (ot % BLOCK_SIZE));
      memcpy(read_buf, buf + buf_index, read_size);
      buf_index += read_size;
      ot += read_size;
      total_size -= read_size;
  }
   
 rv = size;
 return rv;
}

int block_getattr(const char* path, struct stat *st) {
 inode_t* root_dir = get_inode(0);
    dirent_t* root_block = blocks_get_block(root_dir->direct[0]);
    int num_of_entries = root_dir->size / sizeof(dirent_t);
    slist_t* file_name = s_explode(path, '/');
     inode_t* file_inode;

  
    for(int i = 0; i < num_of_entries; i ++) {
        dirent_t current_dir = root_block[i];
        if(strcmp(file_name->data, current_dir.name)) {
            file_inode = get_inode(current_dir.inum); 
            st->st_size = file_inode->size;
            st->st_mode = file_inode->mode;
            st->st_nlink = file_inode->refs;
            return 0;
        }
    }
    return -ENOENT;;
      
}

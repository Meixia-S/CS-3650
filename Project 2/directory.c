#define DIR_NAME_LENGTH 48

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "blocks.h"
#include "inode.h"
#include "slist.h"
#include "directory.h"
#include "bitmap.h"
#include <stdlib.h>

char* get_file_name(char* path){
  char* filename = (char*)malloc(sizeof(path) -1);
 printf("in get_file_name: path is %s\n", path);
  for (int i = 1; i < sizeof(path); i++){
    filename[i -1] = path[i];
  }
  printf("in get_file_name: file name is %s\n", filename);
  return filename;
}
void directory_init() {
  inode_t* root_inode = get_inode(alloc_inode());
  root_inode->mode = 040755;
}

int directory_lookup(inode_t *di, const char *name) {
  inode_t* root_dir = get_inode(0);
    dirent_t* root_block = blocks_get_block(root_dir->direct[0]);
    int num_of_entries = root_dir->size / sizeof(dirent_t);
    slist_t* file_name = s_explode(name, '/');
    inode_t* file_inode;
    printf("in directory_lookup, file name after s_explode is :%s \n", file_name->data);
    for(int i = 0; i < num_of_entries; i ++) {
        dirent_t current_dir = root_block[i];
        if(strcmp(file_name->data, current_dir.name)) {
          printf("in directory_lookup,inum  is :%d \n", current_dir.inum);
          return current_dir.inum;
        }
    }
       return -ENOENT; 
}
  

int directory_put(inode_t *di, const char *name, int inum) {
  // creating a new file
  dirent_t new_directory;
  strncpy(new_directory.name, name, DIR_NAME_LENGTH);
  new_directory.inum = inum;
  new_directory.isFree = 1;
  
  dirent_t* root_dir = blocks_get_block(di->direct[0]);
  int index_directory_entry = di->size / sizeof(dirent_t); // finding out the index of space we are putting in (we assume it is avaliable)
  
  for(int i = 0; i < index_directory_entry; i++) {
    dirent_t current_entry = root_dir[i];
    if(current_entry.isFree == 0) {
      root_dir[i] = new_directory;
      current_entry.isFree == 1;
    }
  }
  // printf("Debug: directory_put(%s, %d)\n", new_directory.name, inum);
  root_dir[index_directory_entry] = new_directory;
  grow_inode(di, sizeof(dirent_t));
   
  return 0;
}

int directory_delete(inode_t *di, const char *name) {
  dirent_t* root_dir = blocks_get_block(di->direct[0]);
  int num_entry = di->size / sizeof(dirent_t);
  
  for (int i = 0; i < num_entry; i ++) {
    dirent_t current_entry = root_dir[i];
    if(strcpy(current_entry.name, name) == 0) {
      current_entry.isFree = 0;
      shrink_inode(di, sizeof(dirent_t));//shrink size of inode of root_dir
      //flip inode of the file to free
      int file_inum = current_entry.inum;
      int* bm = get_inode_bitmap();
      bitmap_put(bm, file_inum, 0); 
      return 0;
    }
  }
  return -ENOENT;
}

slist_t *directory_list(const char *path) {
  slist_t* return_list = NULL;
  // find out which inode is associated to the root directory - it will always be the first inode as we are hard coding that in
  inode_t* root_inode = get_inode(0);
  dirent_t* root_dir = blocks_get_block(root_inode->direct[0]);
  // printf("debug: root_inode->size  is %d\n", root_inode->size );
  int num_entry = root_inode->size / sizeof(dirent_t);
 
  for (int i = 0; i < num_entry; i ++) {
      dirent_t current_entry = root_dir[i];
      s_cons(current_entry.name, return_list);
    }
  
    return return_list;
 
}


void print_directory(inode_t *dd) {
  int number_directory = sizeof(dirent_t);  
  dirent_t* dirs = blocks_get_block(dd->direct[0]);
  
  for (int i = 0; i < number_directory; i ++) {
    printf("Directory ID: %d\n Name: %s\n Inum: %d\n", i, dirs[i].name, dirs[i].inum);
  }
}

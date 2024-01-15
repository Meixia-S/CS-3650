// based on cs3650 starter code

#include <assert.h>
#include <bsd/string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include "inode.h"
#include "directory.h"
#include "blocks.h"
#include "slist.h"

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask) {
  int rv = 0;

  // Only the root directory and our simulated file are accessible for now...
  if (strcmp(path, "/") == 0 || strcmp(path, "/hello.txt") == 0) {
    rv = 0;
  } else { // ...others do not exist
    rv = -ENOENT;
  }

  printf("access(%s, %04o) -> %d\n", path, mask, rv);
  return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
// Implementation for: man 2 stat
// This is a crucial function.
int nufs_getattr(const char *path, struct stat *st) {
  int rv = 0;

  if (strcmp(path, "/") == 0) { // create directory
    st->st_mode = 040755; // directory
    st->st_size = 0;
    st->st_uid = getuid();
    st->st_nlink = 1;
//      printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
//          st->st_size);
//     inode_t* root_dir = get_inode(0);
//     dirent_t* root_block = blocks_get_block(root_dir->direct[0]);
//     int num_of_entries = root_dir->size / sizeof(dirent_t);
//      for(int i = 0; i < num_of_entries; i ++) {
//        printf("get in here 0\n");
//             dirent_t current_dir = root_block[i];
//             if(current_dir.isFree == 1){
//                printf("get in here 1\n");
//                inode_t* file_inode = get_inode(current_dir.inum);
//                printf("get in here 2\n");
//                st->st_size = file_inode->size;
//                 st->st_mode = file_inode->mode;
//                st->st_nlink = file_inode->refs;
//                 printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", current_dir.name, rv, st->st_mode,
//                 st->st_size);
//             }
//      }
//       printf("get in here 3\n");
//       return 0;
  } else { 
    rv = block_getattr(path, st);
    st->st_uid = getuid();
  } 
  
  // if files do not exist on this filesystem
  if (rv < 0) {
    return -ENOENT;
  }
  
  printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
         st->st_size);
  return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  struct stat st;
    int rv = 0;
    char slash = '/';
  //   printf("point 0\n");
    rv = nufs_getattr(&slash, &st);
    slist_t* file_list = directory_list(path);
    while(file_list != NULL){
  //      printf("point 3\n");
  //    printf("the current file name is:%s\n", file_list->data);
  //      printf("point 4\n");
     rv= nufs_getattr(file_list->data, &st);
  //      printf("point 5\n");
     file_list = file_list->next;
    }


    printf("readdir(%s) -> %d\n", path, rv);
    return rv;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
// Note, for this assignment, you can alternatively implement the create
// function.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
int rv = -1;
  rv = block_mknod(path,  0100644);
  printf("mknod(%s, %04o) -> %d\n", path, mode, rv);
  return rv;
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int nufs_mkdir(const char *path, mode_t mode) {
  int rv = nufs_mknod(path, mode | 040000, 0);
  printf("mkdir(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_unlink(const char *path) {
 slist_t* current_name = s_explode(path, '/');
  inode_t* root_dir = get_inode(0);
  int rv;
  
  rv = directory_delete(root_dir, current_name->data); // delete the item from the parent directory
  printf("unlink(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_link(const char *from, const char *to) {
    int rv = -1;
   inode_t* root_dir = get_inode(0);
   int inum = directory_lookup(root_dir, from);
  
    //if file not exist
      if (inum < 0){
       return -ENOENT;
      }
  
    inode_t* file_inode = get_inode(inum);
    slist_t* new_file_name = s_explode(to, '/');
    printf("in nufs_link: new_file_name is %s/n", new_file_name->data);

    rv = directory_put(root_dir, new_file_name->data, file_inode->inum); // delete the item from the parent directory
    printf("link(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

int nufs_rmdir(const char *path) {
  int rv = -1;
  printf("rmdir(%s) -> %d\n", path, rv);
  return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int nufs_rename(const char *from, const char *to) {
  int rv = 0;
  int num1;
  num1 = nufs_link(from, to);
  int num2;
  num2 = nufs_unlink(from);
  
  if(num1 != 0 || num2 != 0) {
    rv = -1;
  }
  
  printf("rename(%s => %s) -> %d\n", from, to, rv);
  return rv;
}

int nufs_chmod(const char *path, mode_t mode) {
  int rv = -1;
  printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
  return rv;
}

int nufs_truncate(const char *path, off_t size) {
  int rv = -1;
  rv = inode_size_adjustor(path, size);
  printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
  return rv;
}

// This is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
// You can just check whether the file is accessible.
int nufs_open(const char *path, struct fuse_file_info *fi) {
  int rv = 0;
  printf("open(%s) -> %d\n", path, rv);
  return rv;
}

// Actually read data
int nufs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
  printf("got here 1");
  int rv = block_read(path, buf, size, offset);
  printf("got here 2");
  printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
  return rv;
}

// Actually write data
int nufs_write(const char *path, const char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi) {
 int rv = block_write(path, buf, size, offset);
  printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
  return rv;
}

// Update the timestamps on a file or directory.
int nufs_utimens(const char *path, const struct timespec ts[2]) {
  int rv = 0;
  printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n", path, ts[0].tv_sec,
         ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
  return rv;
}

// Extended operations
int nufs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
               unsigned int flags, void *data) {
  int rv = -1;
  printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
  return rv;
}

void nufs_init_ops(struct fuse_operations *ops) {
  memset(ops, 0, sizeof(struct fuse_operations));
  ops->access = nufs_access;
  ops->getattr = nufs_getattr;
  ops->readdir = nufs_readdir;
  ops->mknod = nufs_mknod;
  // ops->create   = nufs_create; // alternative to mknod
  ops->mkdir = nufs_mkdir;
  ops->link = nufs_link;
  ops->unlink = nufs_unlink;
  ops->rmdir = nufs_rmdir;
  ops->rename = nufs_rename;
  ops->chmod = nufs_chmod;
  ops->truncate = nufs_truncate;
  ops->open = nufs_open;
  ops->read = nufs_read;
  ops->write = nufs_write;
  ops->utimens = nufs_utimens;
  ops->ioctl = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int main(int argc, char *argv[]) {
  assert(argc > 2 && argc < 6);
 init_file_system(argv[--argc]);
  nufs_init_ops(&nufs_ops);
  return fuse_main(argc, argv, &nufs_ops, NULL);
}

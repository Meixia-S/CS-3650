/**
 * @file blocks.h
 * @author CS3650 staff
 *
 * A block-based abstraction over a disk image file.
 *
 * The disk image is mmapped, so block data is accessed using pointers.
 */
#ifndef BLOCKS_H
#define BLOCKS_H

#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

extern const int BLOCK_COUNT; // we split the "disk" into blocks (default = 256)
extern const int BLOCK_SIZE;  // default = 4K
extern const int NUFS_SIZE;   // default = 1MB

extern const int BLOCK_BITMAP_SIZE; // default = 256 / 8 = 32

/** 
 * Compute the number of blocks needed to store the given number of bytes.
 *
 * @param bytes Size of data to store in bytes.
 *
 * @return Number of blocks needed to store the given number of bytes.
 */
int bytes_to_blocks(int bytes);

/**
 * Load and initialize the given disk image.
 *
 * @param image_path Path to the disk image file.
 */
void blocks_init(const char *image_path);

/**
 * Close the disk image.
 */
void blocks_free();

/**
 * Get the block with the given index, returning a pointer to its start.
 *
 * @param bnum Block number (index).
 *
 * @return Pointer to the beginning of the block in memory.
 */
void *blocks_get_block(int bnum);

/**
 * Return a pointer to the beginning of the block bitmap.
 *
 * @return A pointer to the beginning of the free blocks bitmap.
 */
void *get_blocks_bitmap();

/**
 * Return a pointer to the beginning of the inode table bitmap.
 *
 * @return A pointer to the beginning of the free inode bitmap.
 */
void *get_inode_bitmap();

/**
 * Allocate a new block and return its number.
 *
 * Grabs the first unused block and marks it as allocated.
 *
 * @return The index of the newly allocated block.
 */
int alloc_block();

/**
 * Deallocate the block with the given number.
 *
 * @param bnun The block number to deallocate.
 */
void free_block(int bnum);

/**
 * Making a new inode at the given path location with the given mode.
 *
 * @param path The location in which the new node should be made.
 * @param mode The file permission code.
 */
int block_mknod(const char* path, int mode);

/**
 * Adjusting the size of the inode at the given path with the given amount to adjust.
 * 
 * @param path The path to the inode we want to adjust the size for.
 * @param size The amount we wish to shrink or grow the inode.
 *
 */
int inode_size_adjustor(const char* path, off_t size);

/**
 * Read the given file at the path location.
 *
 * @param path The location of the file we want to read.
 * @param buf The output stream. 
 * @param size Is the amount that needs to be read byte wise.
 * @param offset The byte where the program should start reading.
 */
int block_read(const char* path, const char* buf, size_t size, off_t offset);


/**
 * Write or add content the given file at the path location.
 *
 * @param path The location of the file we want to write in.
 * @param buf The input stream. 
 * @param size Is the amount that needs to be write byte wise.
 * @param offset The byte where the program should start writing.
 */
int block_write(const char* path, const char* buf, size_t size, off_t offset);

/**
 * Creates the two bitmaps we need, allocates space for the blocks that store the metadata, and create the root directory.
 *
 * @param img_path The path in which the root directory is made.
 */
void init_file_system(const char* img_path);

/**
 * Gets the attricute of a file at the given path location.
 * 
 * @param path The location of the file we want to get the attributes for.
 * @param stat A struct the data representation of the information we get from the file.
 */
int block_getattr(const char* path, struct stat *st);

#endif

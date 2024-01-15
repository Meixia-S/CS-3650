<pre>
File System (FS):
  - translateion from files + dirs
  - free space managememt

Design:
  1) Data structures - array, linked lists, structs
  2) Access methods - mapping our representation onto the API

![image](https://github.khoury.northeastern.edu/sindelar/project2/blob/main/image0.jpeg)

Data to represent:
  1) content
  2) tree structure of files + dirs and directories
  3) metedata: size, type, dates, name(s), ownership, access to files, content location(s)
  4) file system meta data
  5) free space tracking 

Disk: 
  - divided into (fixed) blocks
  - 4k (avg. size)
  - blocks (to numbered sequentially)
  
___________________________________________

  - D : data - content
  - I : inode table - metadata
  - S : superblock, file system metadata
  - B : free block bitmap

I-node Table:
  1) size 
  2) block(s)
  3) owner
  4) irration date and time
  5) modification date and time 
  6) access
  7) mode
 How ever many of these we have will be the amount of files that can be stored

How to implement creating a file:
  echo "hello word" > hello.txt
  mkdir ("hello.txt", 0664)
  truncate ("hello.txt", 0)
  write ("hello.txt, "hello world\n", IR (?))
  
  1) allocate i-node                1 - 2 is mkdir
  2) write a dir entry
  3) set size to 0                  3 - 4 is truncate
  4) allocate data block (DB)
  5) write data to DB               5 - 7 is write
  6) associate DB with the i-node
    6.1) by using a bitmap **
  7) update the size 

How to implement growing a file:
  mkdir /foo
  mkdir ("/foo", 0755);
  
  1) find directory to create the dir 
  2) create i-node
  3) associate i-node with a name 
  4) grab a data block
  5) associate the data block with the i-node
  6) write the data block

How to ....
  mknode ("/foo/big.txt", ...)
  write ("/foo/big.txt", "...", 5k)


** 
I-node Bitmap (end)
+---------------------------+
| idx | size | block | type |
+---------------------------+
|  0  |      |   6   |   D  |
|  1  |  12  |   7   |   F  |
|  2  |  --  |   8   |   D  |
|  3  |  5k  | 9, 12 |   F  |
+---------------------------+


nufs_init_ops($nufs_ops)
fuse_main(args, )
</pre>

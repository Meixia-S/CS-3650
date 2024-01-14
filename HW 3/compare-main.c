/* Complete the C version of the driver program for compare. This C code does
 * not need to compile. */

#include <stdio.h>

extern long compare(long num1, long num2);

int main(int argc, char *argv[]) {
  
  if (num1 ==  NULL || num2 == NULL || argv[] > 2) {
    printf("you can only enter in 2 arguments in this program!\n");
    return 1;
  }

  if (num1 > num2) {
    printf("greater\n");
  } else if (num1 == num2) {
    printf("equal\n");
  } else {
    printf("less\n");
  }
  return 0;
}

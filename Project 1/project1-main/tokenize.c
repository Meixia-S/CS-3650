#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "token.h"

#define MAXLENGTH 255

// The demo drive for the functions in token.h, it print each token return by tokenize with a new line character appened
int main(int argc, char **argv) {
  char buffer[256];
  int length = read(0, buffer, 255);
  vect_t* tokens = tokenize(buffer);
  int bound = vect_size(tokens);
	
  for (int i = 0; i < bound ;i++) {
	printf("%s\n", vect_get(tokens,  i));
  }
	
  vect_delete(tokens);
  return 0;
}

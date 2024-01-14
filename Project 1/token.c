#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "token.h"

#define MAXLENGTH 255

// Finding the index of the closing quotation
int helperFinalCharFinder(char* input, int pos, char finalChar) {
  int counter = pos;
  if(finalChar == '"') {
     counter ++;
  }  
  while(counter < strlen(input)) {
    if(input[counter] == finalChar) {	
      return counter;
    }
    counter ++;
  }
return counter;
}


// Determines the length of the normal string (aka no special characters)
int helperEndCharFinder(char* input, int pos) {
  char space = ' ';
  char tab = '\t';
  char newLine = '\n';
  int counter = pos + 1;
  while(counter < strlen(input)) {
    if(input[counter] == space || input[counter] == tab || input[counter] == newLine) {	
	 return counter - 1;
    } else if (input[counter] == ';'){
	 return counter -1;
    }
    counter ++;
  }
  return counter;
}


// Returns a substring of the given input starting from the 'pos' index 
// to the 'pos + len' index.
char* subString(int pos, int len, char* input) {
   char* subString = malloc(len - pos + 2);

  int counter = 0;
  while (pos <= len) {
    char c = input[pos];
    subString[counter] = input[pos];
    pos ++;
    counter ++;
  }
   subString[counter] = '\0';
  return subString;
}


// The main methods that parses the string input into the tokens
// that are seperated with a new line
vect_t* tokenize(char* buffer){
   vect_t * output = vect_new();
   char greaterThan = '>';
   char lessThan = '<';
   char semiColon = ';';
   char pipe = '|';
   char openParen = '(';
   char closeParen = ')';
   char quote = '\"';
   char space = ' ';
   char tab = '\t';
   char newLine = '\n';
   int length = strlen(buffer);
   for(int i = 0; i < length; i++){
     if (buffer[i] == greaterThan || buffer[i] == lessThan ||  buffer[i] == semiColon ||  buffer[i] == pipe ||
	buffer[i] == openParen || buffer[i] == closeParen ) {
	 // Handles the special characters
	 char *stringThatWeCat = malloc(2);
         stringThatWeCat[0] = buffer[i];
	 stringThatWeCat[1]= '\0';
         vect_add(output, stringThatWeCat);
	     
     } else if (buffer[i] == quote) {
	// Ensures all content between quotations as a single token
        int endPos = helperFinalCharFinder(buffer, i, quote);
	// ignoring the index of quote  
	vect_add(output, subString(i+1, endPos-1, buffer));
        i = endPos;

     } else if (buffer[i] != space && buffer[i] != tab && buffer[i] != newLine) {
	// Ensures all regular strings are one token
        int endPos = helperEndCharFinder(buffer, i);
	char* subStr = subString(i, endPos, buffer);
        char copy[strlen(subStr)];
        strcpy(copy, subStr);
        vect_add(output, copy);
        free(subStr);
        i = endPos;
     }
  }
  return output;
}

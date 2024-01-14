#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "token.h"

#define MAXLENGTH 255

// Determines if the given char is the '<' symbol
int inputSymbolDetector(const char* input) {
    int counter = 0;
    if(input[0] == '<') {
       counter ++; 
    }
    return counter;
}


// Determines if the given char is the '>' symbol 
int outputSymbolDetector(const char* input) {
    int counter = 0;
    if(input[0] == '>') {
       counter ++; 
    }
    return counter;
}


// Determines if the given char is the '|' symbol 
int pipeSymbolDetector(const char* input) {
    int counter = 0;
    if(input[0] == '|') {
       counter ++; 
    }
    return counter;
}


// Parses the token and executes the command using another helper
vect_t* pipeChildCommandHelper(vect_t* vect_output, int start, int end){
    vect_t* command = vect_new();
    for(int i = start + 1; i < end; i ++) {
        vect_add(command, vect_get(vect_output, i));
    }
    return command;
}


// This helper helps to parse the user input to redirect either input or output!
vect_t* inputOutputRedirectHelper(vect_t* vect_output, int indexOfSymbol,const char* symbol) {
    const char*  fileS = vect_get(vect_output, indexOfSymbol + 1);
    
    if(symbol[0] == '<') {
        if (close(0) == -1) {
            perror("Error closing stdout\n");
            exit(1);
        }
        int fd = open(fileS, O_RDONLY);
        assert(fd == 0);
    } else {
         if (close(1) == -1) {
            perror("Error closing stdout\n");
            exit(1);
        }
        int fd = open(fileS, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        assert(fd == 1);
    } 
}


// A helper that used by pipeHelper to execute a singke commad. It read the command line representation ,
// which is a vect_t and assign them to a buffer string for execution. Check if the command contains any  
// redirect symbol in this process, call the helper to modify file disciptor if there is.
void pipeExecuteSingleCommandHelper(vect_t* vect_output) {
    char binString[] = "/bin/";
    char* input[vect_size(vect_output)];
    input[0] = (char *)malloc(sizeof(vect_get(vect_output, 0)) + strlen(binString));
    input[0] = binString;	
    strcat(input[0], vect_get(vect_output, 0));
    int bound = vect_size(vect_output);
	
    for(int i = 1; i < bound; i ++) {
	    
	if(inputSymbolDetector(vect_get(vect_output, i)) == 1) {
            inputOutputRedirectHelper(vect_output, i, vect_get(vect_output, i));
            input[i] = NULL;
            break;
        }
	  
        if(outputSymbolDetector(vect_get(vect_output, i)) == 1) {
           inputOutputRedirectHelper(vect_output, i, vect_get(vect_output, i));
           input[i] = NULL;
           break;
        }
        input[i] = vect_get_copy(vect_output, i);
    }
    input[vect_size(vect_output)] = NULL;
    execvp(input[0], input);
    exit(1);
}


/*
  a helper called by pipeHelper to connect the child with a centain write end of the pipe that connects to the parent.
  If the child is not the last child in this pipe-group command, creat another pipe and a child of the child.
*/
void pipeGenerator(vect_t* vect_output, int pipeChildNum, int* indexes, int write_fd) {
    vect_t* childCommand = vect_new();
    int size = vect_size(vect_output);

    if (pipeChildNum ==1){ // still in child
	    if (close(1) == -1) {
            perror("Error closing stdout");
	        exit(1);
	    }  
	    assert(dup(write_fd) == 1);
	    // excute command here
	    childCommand = pipeChildCommandHelper(vect_output, indexes[0], indexes[1]);
	    pipeExecuteSingleCommandHelper(childCommand);
    } else { //still in child, but it about to have its own child
	    int pipe_fdsSecond[2]; 
	    if(pipe(pipe_fdsSecond) == -1){
	       perror("Error: ");
	       exit(1);
	    } 
	    int read_fdSecond = pipe_fdsSecond[0]; // index 0 is the "read end" of the pipe
	    int write_fdSecond = pipe_fdsSecond[1]; // index 1 is the "write end" of the pipt
	    int child_status;
	    int child_pid = fork();

	    if (child_pid > 0) { // in parent (that this child have become
	        close(write_fdSecond);
	        if ( close(0) == -1) {
                perror("Error closing stdin");
	            exit(1);
	        }  
	        assert(dup(read_fdSecond) == 0);
	        if (close(1) == -1) {
                perror("Error closing stdout");
	            exit(1);
	        }  
	        assert(dup(write_fd) == 1);
	        // excute command here
	        childCommand = pipeChildCommandHelper(vect_output, indexes[pipeChildNum-1], indexes[pipeChildNum]);
	        wait(&child_status);	
	        pipeExecuteSingleCommandHelper(childCommand);
	    } else if (child_pid == 0) { // in child
	    close(read_fdSecond); // close the other end of the pipe
	    pipeGenerator( vect_output, pipeChildNum -1, indexes, write_fdSecond);
	    }
      }
}


// Make the left most command be the parent of the child that handle command to it right, create
// a pipe and connect the read end of the pipe, call the helper to create its child.
void pipeHelper(int pipeChildNum, int* indexes, vect_t* vect_output) {
    vect_t* parentCommand = vect_new();
    int pipe_fds[2]; 
    assert(pipe(pipe_fds) == 0); // returns 0 on success

    int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
    int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipt
    int child_status;
    int child_pid = fork();
  
    if (child_pid > 0) { // in parent
        close(write_fd);

        // replace stdout with the write end of the pipe 
        if (close(0) == -1) {
            perror("Error closing stdout");
            exit(1);
        }
      
        assert(dup(read_fd) == 0);

        // execute command.
        parentCommand = pipeChildCommandHelper(vect_output, indexes[pipeChildNum-1], vect_size(vect_output));
        wait(&child_status);
        pipeExecuteSingleCommandHelper(parentCommand);
    } else if (child_pid == 0) { // in child
	close(read_fd); // close the other end of the pipe
        pipeGenerator( vect_output,pipeChildNum -1, indexes, write_fd);
    } else {
        perror("Error - fork failed");
        exit(1);
    }
}
 

// Takes the input and transforms it into a char array to execute the commands and exit
void inputGenerator(vect_t* vect_output) {
    char binString[] = "/bin/";
    char* input[vect_size(vect_output)];
    int bound = vect_size(vect_output);
    int indexes[bound];
    indexes[0] = -1;
    int pipeChildNum;
    pipeChildNum = 1;
    int child_status;
    int redirectSymboxIndex;
    redirectSymboxIndex = 0; 

    input[0] = (char *)malloc(sizeof(vect_get(vect_output, 0)) + strlen(binString));
    input[0] = binString;	
    strcat(input[0], vect_get(vect_output, 0));
    for (int i = 1; i < vect_size(vect_output) ; i ++) {
         
        if (inputSymbolDetector(vect_get(vect_output, i)) == 1 ||outputSymbolDetector(vect_get(vect_output, i)) == 1) {
	        redirectSymboxIndex = i;
        } else if (pipeSymbolDetector(vect_get(vect_output, i)) == 1) {
            indexes[pipeChildNum] = i;
            pipeChildNum ++;
        }
	input[i] = vect_get_copy(vect_output, i); 
    }
   
    if(pipeChildNum > 1) {
	int child_pid = fork();
	    
	    if(child_pid == 0){
                pipeHelper(pipeChildNum, indexes, vect_output);
	        exit(1);
	    } else {
	        wait(&child_status);
	        exit(0);
	    }
    }

    if (redirectSymboxIndex != 0) {
        inputOutputRedirectHelper(vect_output, redirectSymboxIndex, vect_get(vect_output, redirectSymboxIndex));
        input[redirectSymboxIndex] = NULL;  
    } else {
	    input[vect_size(vect_output)] = NULL;
    }
    execvp(input[0], input);
    exit(1);
}


// Takes in the input from the user as a vector and the previous command vector.
// Then parses and reads the input and handles all the builtIn commands.
FILE* executeCommandHelper(vect_t* vect_output, vect_t* prevCommand) {
    int source = strcmp(vect_get(vect_output, 0),  "source");
    int result = strcmp(vect_get(vect_output, 0),  "exit");
    int help = strcmp(vect_get(vect_output, 0),  "help");
    int prev = strcmp(vect_get(vect_output, 0),  "prev");
    int cd = strcmp(vect_get(vect_output, 0),  "cd");
    int bound;
    FILE *file;
    file = stdin;

    if(result == 0) { exit(2); } 
    
    if (prev == 0) {
        vect_output = prevCommand;
        char* prevString = malloc(sizeof(char) * vect_size(prevCommand));
        char* space = malloc(sizeof(char) * 3);
	space = " ";
        if(prevCommand == NULL) {
            printf("You do not have a previous command\n");
            exit(0);
        }
        bound = vect_size(prevCommand);
        for (int i = 0; i < bound ;i++){
            strcat(prevString, vect_get(prevCommand, i));
            strcat(prevString, space);
        }
	printf("%s\n", prevString);    
        source = strcmp(vect_get(prevCommand, 0),  "source");
        help = strcmp(vect_get(prevCommand, 0),  "help");
        cd = strcmp(vect_get(prevCommand, 0),  "cd");
    } 
       
    if(help == 0) {
        char* helpMessage = ("cd: this command will change the current working directory of the shell to the path specified as the argument \n"
                              "source: execute a script (takes a filename as an argument and processes each line of the file as a command, including built-ins) \n"
                              "prev: prints the previous command line and executes it again, without becoming the new command line \n"
                              "help: explains all the built-in commands available in your shell \n");
    printf("%s", helpMessage);
    prevCommand = vect_output;
    exit(0); }
      
    if (cd == 0) {
        exit(3); 
    }
     
    if (source == 0) {
        file = fopen(vect_get(vect_output, 1), "r");
        if(file == NULL) {
            printf("Error: this file is empty");
            exit(0); }
        return file;
    }
    inputGenerator(vect_output);
}


// Parses the token and executes the command using another helper
void sequenceHelper(vect_t* vect_output, int start, int end){
    vect_t* command = vect_new();
    int child_status;
    
    for(int i = start; i < end; i ++) {
        vect_add(command, vect_get(vect_output, i));
    }
    int child_pid = fork(); 
	
    if(child_pid == 0) {
        executeCommandHelper(command , NULL);
    } else {
        wait(&child_status);
    }
}


// Handles sequencing the commands by detecting it and calling a helper
// that executes the command for every ; detected
int sequenceHandler(vect_t* vect_output, int bound) {
    int counter;
    counter = 1;
    int indexes[bound];
    indexes[0] = -1;
    
    for(int i = 0; i < bound; i++){
        char *currVectData = vect_get_copy(vect_output, i);
        if(currVectData[0] == ';'){
            indexes[counter] = i;
            counter ++;
        }
    }
  
    if(counter > 1) {
        for (int i = 0; i < counter - 1; i ++) {
            sequenceHelper(vect_output, indexes[i] + 1, indexes[i + 1]);
        }
        sequenceHelper(vect_output, indexes[counter - 1] + 1, vect_size(vect_output));
        printf("shell $ ");
        return 1;
    }
    return 0;
}


// Check if the input command is a sequence command. return the last effective command if it is. If it 
// is not a sequence command return the whole command as the previous command;
vect_t* updeateSequncePre(vect_t* vect_output, int bound) {
    int lastSequenceIndex;
    lastSequenceIndex = 0;
    vect_t* prevCommand;
	
    for(int i = 0; i < bound; i++){
        char *currVectData = vect_get_copy(vect_output, i);
        if(currVectData[0] == ';'){
          lastSequenceIndex = i;
        }
    }
	
    if(lastSequenceIndex > 0) {
	prevCommand= vect_new();
        for (int i = lastSequenceIndex + 1 ; i < bound ; i ++) {
           vect_add(prevCommand, vect_get_copy(vect_output, i));
	}
    } else {
	   prevCommand = vect_output;
    } 
    return prevCommand;
}


/* The function that runs the program - 
   Creates a shell within the shell:
      - executes builtIn commands found in a standard shell
      - executes four extra commands (builtIn commands for our shell)
      - executes four advanced shell features:
            - Sequencing
            - Input redirection
            - Output redirection
            - Pipes 
*/
int main(int argc, char **argv) {
    char welcomeMessage[] = "Welcome to mini-shell\n";
    char binString[] = "/bin/";
    char buffer[MAXLENGTH];
    vect_t* prevCommand;
    int child_status;
    int prev;	
    FILE *file;
    file = stdin;
    
    assert(write(1, welcomeMessage, strlen(welcomeMessage)) == strlen(welcomeMessage));
    printf("shell $ ");
      
    while(fgets(buffer, MAXLENGTH, file)) {
	if(strlen(buffer)==1){
           printf("shell $ ");
	   continue;
	}
        vect_t* vect_output = tokenize(buffer);
        int bound = vect_size(vect_output);
      
        if(sequenceHandler(vect_output, bound) == 1) {	
	    prevCommand = updeateSequncePre(vect_output, vect_size(vect_output));
            continue;
        }
      
        int child_pid = fork(); 
        if(child_pid == 0) {
            file = executeCommandHelper(vect_output, prevCommand);			
            continue; 
        } else {
            wait(&child_status);		
            if (WEXITSTATUS(child_status) == 3) {
                chdir(vect_get(vect_output, 1));
		printf("\nshell $ ");
		vect_delete(vect_output);
		continue;
            } 
            if (WEXITSTATUS(child_status) == 2) {
                printf("Bye bye.\n");
		vect_delete(vect_output);
		exit(0);
            } else if (WIFEXITED(child_status) && WEXITSTATUS(child_status) == 1 ) {
                printf("[%s]: command not found", vect_get(vect_output, 0));
		vect_delete(vect_output);
                printf("\nshell $ ");
            }
            
	    prev = strcmp(vect_get(vect_output, 0),  "prev");
            if (prev != 0) {	
                prevCommand = updeateSequncePre(vect_output, vect_size(vect_output));
            }

            if (file == stdin) {
                printf("shell $ ");
            }
        }
    }
    if (file != stdin){
        fclose(file);
        file = stdin;
    } else {
      printf("\nBye bye.\n");
      fclose(file);
      file = stdin;
      return 0;
    }
}

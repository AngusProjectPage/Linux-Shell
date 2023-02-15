#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "main.h"

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE], *token;
int main(int argc, char **argv) {
    loop_shell();
    return 0;
}

void loop_shell() {
    do{
        display();
        start_fork();
        if(feof(stdin)){
            break;
        }

    } while(strcmp(buffer, "quit\n") != 0 );
}

void display(){
    char cwd[256];
    getcwd(cwd, 256);
    printf("%s>>> ", cwd);
}

int start_fork() {
    char *arguments[50];
    read_parse(arguments); 
    pid_t pid;
    pid = fork();
    if(pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* Child Process */ 
        execvp(arguments[0],arguments);
    }
    else { /* parent process */
        /* parent will wait for the child process to complete*/
        wait(NULL);
        printf("Child complete");
        return 0;
    }
}

void read_parse(char** arguments){
   
    char delim[] = " "; // Each token to be split by whitespace 
    char *c;
    char *t;
    int i = 0;
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        }
        else {
                char *token = strtok(buffer, delim);
                while(token != NULL) {
                    token = strtok(NULL, delim);
                    arguments[i] = malloc(strlen(token) + 1);
                    arguments[i] = token;
                    i++;
                }
                arguments[i] = NULL;
            } 
    }
    


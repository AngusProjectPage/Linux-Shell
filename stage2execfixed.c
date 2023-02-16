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
    setenv("PATH","/bin",0);
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
    pid_t pid;
    pid = fork();
    if(pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    else if (pid == 0) { /* Child Process */
        read_parse(arguments); 
        if (execvp(arguments[0], arguments) < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else { /* parent process */
        /* parent will wait for the child process to complete*/
        wait(NULL);
        return 0;
    }
}

void read_parse(char** arguments) {

    char delim[] = " "; // Each token to be split by whitespace 
    char *c;
    char *t;
    int i = 0;
    for (i = 0; i < 50; i++){
        arguments[i] = NULL;
}
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {

            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        }
        else {
            char *token = strtok(buffer, delim);
            while (token != NULL) {
                arguments[i] = malloc(strlen(token) + 1);
                strcpy(arguments[i], token);
                token = strtok(NULL, delim);
                i++;
            }
        }

            arguments[i] = NULL;
        }

    


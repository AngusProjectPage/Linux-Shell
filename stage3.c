#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "main.h"

#define BUFFER_SIZE 512
char *arguments[50];
char buffer[BUFFER_SIZE], *token;
char *envPath;
char *homePath;
char *currentPath;

int main(int argc, char **argv) {
    envPath = getenv("PATH");
    homePath = getenv("HOME");
    if(homePath != NULL){
        chdir(homePath);
        display();
    }
    else{
        printf("invalid");
    }
    
    loop_shell();
    return 0;
}

void loop_shell() {
    do{
        display();
        read_parse();
        if(feof(stdin)){
            break;
        } else if(strcmp(arguments[0], "getpath") == 0) {
            printf("%s", getenv("PATH"));
        } else if(strcmp(arguments[0], "setpath") == 0) {
            //chdir(arguments[1]);
        } else if(strcmp(arguments[0], "exit") == 0) {
            //setenv(homePath);
        } else {
            start_fork();
        }
        free(*arguments);
    } while(strcmp(buffer, "exit") != 0 );
}

void display(){
    char cwd[256];
    getcwd(cwd, 256);
    printf("%s>>> ", cwd);
}


void read_parse(){
    char delim[] = " \n\t()<>|&;"; // Each token to be split by whitespace 
    char *c;
    char *t;
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        }
        else {
            int i = 0;
            char *token = strtok(buffer, delim);
            while(token != NULL && i < 49) {
                arguments[i] = malloc(strlen(token));
                strcpy(arguments[i], token);
                token = strtok(NULL, delim);
                i++;
            }
            arguments[i] = NULL;
        } 
    }

int start_fork() {
    pid_t pid;
    pid = fork();
    if(pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* Child Process */  
        if (execvp(arguments[0], arguments) < 0) {
            if((strcmp(arguments[0], "exit"))) {
                perror(arguments[0]);
            }     /* execute the command  */
            exit(1);
        }
    }
    else { /* parent process */
        /* parent will wait for the child process to complete*/
        wait(NULL);
        return 0;
    }
}




    


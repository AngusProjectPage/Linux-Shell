#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "main.h"

typedef struct previousCommands {
    int commandNumber;
    char* string;
} previousCommands;

#define BUFFER_SIZE 512
char *arguments[50];
char buffer[BUFFER_SIZE];
char *envPath;
char *homePath;
int commandCounter = 0;
previousCommands commands[20];


int main(int argc, char **argv) {
    home();
    loop_shell();
    return 0;
}

void home(){
    envPath = getenv("PATH");
    homePath = getenv("HOME");
    if(homePath != NULL){
        chdir(homePath);
    }
    else{
        printf("invalid");
    }
}
void loop_shell() {
    do{
        free(*arguments);
        display();
        readInput();
        char* command = parseInput();
        trackHistory(command);
    } while(strcmp(arguments[0], "exit") != 0 );
}

void display(){
    char cwd[256];
    getcwd(cwd, 256);
    printf("%s>>> ", cwd);
}

void readInput(){
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

char* parseInput() {
    if(feof(stdin) || strcmp(arguments[0], "exit") == 0){ //<CTRL+D> OR "EXIT" to close shell
        setenv("PATH", envPath, 1);
        printf("%s\n", getenv("PATH"));
    } else if(strcmp(arguments[0], "getpath") == 0) { 
        getPath();
        return "getpath";
    } else if(strcmp(arguments[0], "setpath") == 0) {
        setPath();
        return "setpath";
    }
      else if (strcmp(arguments[0],"cd")== 0) {
        changeDirectory();
        return "cd";
    } else if(strcmp(arguments[0], "!?")) {
            int indexNum = commandCounter-1 - 
            arguments[0] = commands->commandNumber;
            parseInput();
        if(strcmp(arguments[0], "!!")) {
            
        } else {

        }
    }
    else {
        startFork();
        return "";
    }
}

void getPath() {
    if(arguments[1] == NULL) {
        printf("Please enter a path \n");
    }
    else if(getenv(arguments[1]) == NULL) {
        fprintf(stderr, "Could not find environment variable '%s'\n", arguments[1]);
    } 
    else if(arguments[2] != NULL) {
        printf("Too many arguments\n");
    }
    else {
        printf("%s\n", getenv(arguments[1]));
    }
}

void setPath() {
    if(arguments[2] != NULL) {
        fprintf(stderr, "Function should take only one parameter\n");
    } else if(access(arguments[1], F_OK == -1)) {
        fprintf(stderr, "Path '%s' does not exist\n", arguments[1]);
    } else if(setenv("PATH", arguments[1], 1) == 0) {
        printf("Path is now %s\n", arguments[1]);
    } else {
        fprintf(stderr, "Error occurred when setting path '%s'\n", arguments[1]);
    }
}

void changeDirectory() {
    if(arguments[1] == NULL){
        home();
    }
    else if(access(arguments[1],F_OK != 0)){
        perror("error");
    }
    else if(arguments[2] != NULL){
        perror("Too many arguments");
    }
    else{
        chdir(arguments[1]);
    }
}

void trackHistory(char* command) {
    if(commandCounter % 20 != 0) {
        for(int i=0; i<20; i++) {
            if(command[i] != NULL) {
                previousCommands newCommand;
                newCommand.commandNumber = commandCounter+1;
                newCommand.string = command;
                command[i] = command;
                break;
            }
        }
    }
    commandCounter++;
}

int startFork() {
    pid_t pid;
    pid = fork();
    if(pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* Child Process */  
        if (execvp(arguments[0], arguments) < 0) {
                perror(arguments[0]);
                exit(1);
        }
    }
    else { /* parent process */
        /* parent will wait for the child process to complete*/
        wait(NULL);
        return 0;
    }
}




    


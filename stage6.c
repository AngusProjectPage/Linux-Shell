#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "main.h"

typedef struct previousCommands {
    int commandNumber;
    char *string[50];
} previousCommands;

#define BUFFER_SIZE 512
char *arguments[50];
char delim[] = " \n\t()<>|&;"; // Each token to be split by whitespace
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
    if (access(".hist_list", F_OK) == 0) {
        loadHistory();
    } 
}

void loop_shell() {
    do{
        display();
        readInput();
        if(feof(stdin)) {
            writeHistory();
            break;
        } 
        parseInput();
    } while(arguments[0] == NULL || strcmp(arguments[0], "exit") != 0);
}

void display() {
    char cwd[256];
    getcwd(cwd, 256);
    printf("%s>>> ", cwd);
}

void readInput(){
    char *t;
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        } 
        int i = 0;
        char *token = strtok(buffer, delim);
        while(token != NULL && i < 49) {
            arguments[i] = malloc(strlen(token) + 1);
            printf("%s", arguments[0]);
            strcpy(arguments[i], token);
            token = strtok(NULL, delim);
            i++;
        }
        arguments[i] = NULL;
    }

void parseInput() {
    if(arguments[0] != NULL) {
        if (feof(stdin) || strcmp(arguments[0], "exit") == 0) { //<CTRL+D> OR "EXIT" to close shell
            setenv("PATH", envPath, 1);
            writeHistory();
        } else if (strcmp(arguments[0], "getpath") == 0) {
            getPath();
            trackHistory();
        } else if (strcmp(arguments[0], "setpath") == 0) {
            setPath();
            trackHistory();
        } else if (strcmp(arguments[0], "cd") == 0) {
            changeDirectory();    
            trackHistory();        
        } else if (strcmp(arguments[0], "!!") == 0) {
            if (commandCounter == 0) {
                printf("No commands in history");
            } else {
                int i = 0;
                previousCommands command = commands[(commandCounter - 1) % 20];
                while (command.string[i] != NULL) {
                    arguments[i] = malloc(strlen(command.string[i]));
                    strcpy(arguments[i], command.string[i]);
                    i++;
                }
                arguments[i] = NULL;
                parseInput();
            }
        } else if (arguments[0][0] == '!') {
            int commandNum = atoi(&arguments[0][1]);
            if(commandNum == 0 || commandNum > commandCounter) {
                printf("Invalid number, commands range from 1 to number of executed commands");
            } else {
                int i = 0;
                previousCommands command = commands[(commandNum-1) % 20];
                while(command.string[i] != NULL) {
                    arguments[i] = malloc(strlen(command.string[i]));
                    strcpy(arguments[i], command.string[i]);
                    i++;
                }
                arguments[i] = NULL;
                parseInput();
            }
            } else if(strcmp(arguments[0], "history") == 0) {
                printHistory();
            } else {
                trackHistory();
                startFork();
            }
        }
    }

    void getPath() {
        if (arguments[1] == NULL) {
            printf("Please enter a path \n");
        } else if (getenv(arguments[1]) == NULL) {
            fprintf(stderr, "Could not find environment variable '%s'\n", arguments[1]);
        } else if (arguments[2] != NULL) {
            printf("Too many arguments\n");
        } else {
            printf("%s\n", getenv(arguments[1]));
        }
    }

    void setPath() {
        if (arguments[2] != NULL) {
            fprintf(stderr, "Function should take only one parameter\n");
        } else if (access(arguments[1], F_OK == -1)) {
            fprintf(stderr, "Path '%s' does not exist\n", arguments[1]);
        } else if (setenv("PATH", arguments[1], 1) == 0) {
            printf("Path is now %s\n", arguments[1]);
        } else {
            fprintf(stderr, "Error occurred when setting path '%s'\n", arguments[1]);
        }
    }

    void changeDirectory() {
        if (arguments[1] == NULL) {
            home();
        } else if (access(arguments[1], F_OK != 0)) {
            perror("error");
        } else if (arguments[2] != NULL) {
            perror("Too many arguments");
        } else {
            chdir(arguments[1]);
        }
    }

    void trackHistory() {
        previousCommands newCommand = commands[commandCounter % 20];
        newCommand.commandNumber = commandCounter + 1;
        int i = 0;
        while (arguments[i] != NULL) {
            newCommand.string[i] = malloc(strlen(arguments[i]) + 1);
            strcpy(newCommand.string[i], arguments[i]);
            i++;
        }
        newCommand.string[i] = NULL;
        commands[commandCounter % 20] = newCommand;
        commandCounter++;
    }

void printHistory() {
    int i, start = 0;
    if (commandCounter > 20) {
        start = commandCounter % 20;
    }
    for (i = start; i < commandCounter; i++) {
        printf("%d ", commands[i % 20].commandNumber);
        int j = 0;
        while (commands[i % 20].string[j] != NULL) {
            printf("%s ", commands[i % 20].string[j]);
            j++;
        }
        printf("\n");
    }
}


int startFork() {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Failed");
            return 1;
        } else if (pid == 0) { /* Child Process */
            if (execvp(arguments[0], arguments) < 0) {
                perror(arguments[0]);
                exit(1);
            }
        } else { /* parent process */
            /* parent will wait for the child process to complete*/
            wait(NULL);
            return 0;
        }
    }

void writeHistory() {
    FILE *fp;
    fp = fopen(".hist_list", "w");
    if(fp == NULL) {
        printf("Error opening file");
        exit(1);
    }
    int i, start = 0;
    if (commandCounter > 20) {
        start = commandCounter % 20;
    }
    for(int i = start; i < commandCounter; i++) {
        int j = 0;
        fprintf(fp, "%d ", commands[i % 20].commandNumber);
        while (commands[i % 20].string[j] != NULL) {
            fprintf(fp, "%s ", commands[i % 20].string[j]);
            j++;
        }
        fprintf(fp, "\n");   
    }
    fclose(fp);
}

void loadHistory() {
    FILE *fp;
    fp = fopen(".hist_list", "r");
    if(fp == NULL) {
        printf("Error opening file");
        exit(1);
    }
    while(fgets(buffer, BUFFER_SIZE, fp)) {
        previousCommands newCommand = commands[commandCounter % 20];
        char *token = strtok(buffer, delim);
        newCommand.commandNumber = atoi(token);
        token = strtok(NULL, delim);
        int i = 0;
        while (token != NULL && i < 49) {
            newCommand.string[i] = malloc(strlen(token) + 1);
            strcpy(newCommand.string[i], token);
            token = strtok(NULL, delim);
            i++;
        }
    }
    fclose(fp);

     previousCommands newCommand = commands[commandCounter % 20];
        newCommand.commandNumber = commandCounter + 1;
        int i = 0;
        while (arguments[i] != NULL) {
            newCommand.string[i] = malloc(strlen(arguments[i]) + 1);
            strcpy(newCommand.string[i], arguments[i]);
            i++;
        }
        newCommand.string[i] = NULL;
        commands[commandCounter % 20] = newCommand;
        commandCounter++;


}



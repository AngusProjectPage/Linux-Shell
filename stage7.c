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

typedef struct aliasStruct{
    char *aliasName;
    char *commandName[50];
} aliasStruct;

#define BUFFER_SIZE 512
char *arguments[50];
char delim[] = " \n\t<>|&;"; // Each token to be split by whitespace
char buffer[BUFFER_SIZE];
char *envPath;
char *homePath;
int aliasCounter = 0;
int commandCounter = 0;
previousCommands commands[20];
aliasStruct aliases[10];

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
    else if(homePath == NULL) {
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
        fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while((c = getchar() != '\n') && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        } 
        int i = 0;
        char *token = strtok(buffer, delim);
        while(token != NULL && i < 49) {
            arguments[i] = malloc(strlen(token) + 1);
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
                trackHistory();
                printHistory();
            } else if(strcmp(arguments[0], "alias") == 0) {
                insertAlias();
            } else if(strcmp(arguments[0], "unalias") == 0){
                removeAlias();
            }
            else {
                trackHistory();
                invokeAlias();
                startFork();
            }
        }
    }

    void getPath() {
        if (arguments[1] == NULL) {
            printf("%s\n", getenv("PATH"));
        } else {
            printf("Too many arguments\n"); 
        }
    }

    void setPath() {
        if (arguments[2] != NULL) {
            fprintf(stderr, "Function should take only one parameter\n");
        } else if (access(arguments[1], F_OK == -1)) {
            fprintf(stderr, "Path '%s' does not exist, please enter in a path\n", arguments[1]);
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
        }   /* parent process */
            /* parent will wait for the child process to complete*/
            wait(NULL);
            return 0;
    }

void writeHistory() {
    FILE *fp;
    fp = fopen(".hist_list", "w");
    if(fp == NULL) {
        printf("Error opening file");
        exit(1);
    }
    int start = 0;
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
    FILE *fp = fopen(".hist_list", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    char line[BUFFER_SIZE];
    int i = 0;
    while (fgets(line, BUFFER_SIZE, fp) != NULL) {
        int j = 0;
        char *token = strtok(line, delim);
        commands[i].commandNumber = atoi(token);
        token = strtok(NULL, delim);
        while (token != NULL) {
            commands[i].string[j] = malloc(strlen(token) + 1);
            strcpy(commands[i].string[j], token);
            token = strtok(NULL, delim);
            j++;
        }
        i++;
    }
    commandCounter = i;
    //fclose(fp);
}

void insertAlias() {
    if(aliasCounter == 10) {
        printf("Max number of aliases have been entered please remove before inserting");
    }
    else if(arguments[1] != NULL && arguments[2] != NULL) {
        aliasStruct newAlias = aliases[aliasCounter];
        newAlias.aliasName = strdup(arguments[1]);
        int argumentsCounter = 2;
        int commandNameCounter = 0;
        while(arguments[argumentsCounter] != NULL) {
            newAlias.commandName[commandNameCounter] = strdup(arguments[argumentsCounter]);
            commandNameCounter++;
            argumentsCounter++;
        }
        aliases[aliasCounter] = newAlias;
        aliasCounter++;
    } else if(arguments[1] != NULL && arguments[2] == NULL) {
        printf("Alias must be entered in format 'alias <command> <newCommandName>'\n");
    }
    else if(aliasCounter != 0) {
        for(int i=0; i<10; i++) {
            if(aliases[i].aliasName != NULL) {
                int counter = 0;
                printf("Alias name: %s, Command: ", aliases[i].aliasName);
                while(aliases[i].commandName[counter] != NULL) {
                    printf("%s ", aliases[i].commandName[counter]);
                    counter++;
                }
                printf("\n");
            }
        }
    } else {
        printf("There are currently no set aliases\n");
    }
}
void removeAlias() {
    if (arguments[1] == NULL) {
        printf("Please enter an alias name\n");
    } else {
        int i;
        int  found = 0;
        for (i = 0; i < 10; i++) {
            if(aliases[i].aliasName != NULL) {
                if (strcmp(aliases[i].aliasName, arguments[1]) == 0) {
                    free(aliases[i].aliasName);
                    free(aliases[i].commandName[0]);
                    while (i < aliasCounter - 1) {
                        aliases[i] = aliases[i + 1];
                        i++;
                    }
                    aliasCounter--;
                    found = 1;
                    break;
                }
            }
        }
        if (found) {
            printf("Alias '%s' removed\n", arguments[1]);
        } else {
            printf("Alias '%s' not found\n", arguments[1]);
        }
    }

}

void invokeAlias(){
    for (int i = 0; i < aliasCounter; i++) {
        if (strcmp(arguments[0], aliases[i].aliasName) == 0) {
            int j = 0;
            while (aliases[i].commandName[j] != NULL && j < 49) {
                arguments[j] = malloc(strlen(aliases[i].commandName[j]) + 1);
                strcpy(arguments[j], aliases[i].commandName[j]);
                j++;
            }
            arguments[j] = NULL;
        }
    }
}

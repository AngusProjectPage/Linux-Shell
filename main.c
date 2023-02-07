#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 512
void loop_shell();
void main(int argc, char **argv) {
    loop_shell();
}

void loop_shell() {
    char buffer[BUFFER_SIZE], *exit, *token;
    char delim[] = " "; // Each token to be split by whitespace 
    char cwd[256];
    char *c;
    getcwd(cwd, 256);
    do {
        printf("%s>>> ", cwd);
        char* t;
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        }
        else {
            if(!feof(stdin)) {
                token = strtok(buffer, delim);
                while(token != NULL) {
                    token = strtok(NULL, delim);
                }
            } else {
                break;
            }
        }
    } while(strcmp(buffer, "quit\n") != 0); // strcmp returns 0 when it's two string are equal
}




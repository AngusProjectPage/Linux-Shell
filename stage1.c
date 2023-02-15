#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE], *token;
int main(int argc, char **argv) {
    loop_shell();
    return 0;
}

void display(){
    char cwd[256];
    getcwd(cwd, 256);
    printf("%s>>> ", cwd);
}



void loop_shell() {

    do{
        display();
        read_parse();
        if(feof(stdin)){
            break;
        }

    } while(strcmp(buffer, "quit\n") != 0 );
}

void read_parse(){
   
    char delim[] = " "; // Each token to be split by whitespace 
    
    char *c;
    char* t;
        t = fgets(buffer, BUFFER_SIZE, stdin);
        if(buffer[strlen(buffer) - 1] != '\n' && !feof(stdin)) {
            char c;
            while(c = getchar() != '\n' && c != EOF);
            printf("input should not exceed 512 characters, try again\n");
        }
        else {
            
                token = strtok(buffer, delim);
                while(token != NULL) {
                    token = strtok(NULL, delim);
                }
            } 
        }
    




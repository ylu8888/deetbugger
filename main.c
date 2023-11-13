#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"

int main(int argc, char *argv[]) {

    log_startup(); //starts up deets

    log_prompt(); // issues a prompt

    char* buffer = NULL; //store the user input into a buffer
    size_t buffSize = 0;

    for(;;){
        fprintf(stdout, "deet> ");

        getline(&buffer,&buffSize,stdin);   //use get line to get user input, getline reallocs buffer each time its called, no need to free buffer

        log_input(buffer); // read a line of input from user

        if(strcmp(buffer, "\n") == 0){ //if user just enters a blank line
            log_prompt(); // issues another prompt
            fflush(stdout);
            continue;
        }

        //iterate through the buffer to see what the user input and checkk if its a valid input or not
        //the user input should match the terminal commands

        //split the input based on spaces using strtok
        char* token = strtok(buffer, " ");

        while(token != NULL){

            //compare the strings, if returns 0 its equal
            if(strcmp(token, "quit") == 0){

            } 
            else if(strcmp(token, "show") == 0){

            }
            else if(strcmp(token, "run") == 0){

            }
            else if(strcmp(token, "stop") == 0){

            }
            else if(strcmp(token, "cont") == 0){

            }
            else if(strcmp(token, "release") == 0){

            }
            else if(strcmp(token, "wait") == 0){

            }
            else if(strcmp(token, "kill") == 0){

            }
            else if(strcmp(token, "peek") == 0){

            }
            else if(strcmp(token, "poke") == 0){

            }
            else if(strcmp(token, "bt") == 0){

            }
            else{
                //IF None of the commands match the user input, its an error
                log_error(token); //send error msg with token
                fprintf(stdout, "?\n");

            }

            token = strtok(NULL, " ");

        }




        log_prompt(); // issues another prompt

        fflush(stdout); //fflush the stdout after every time the user prints something
    }


    return 0;
}

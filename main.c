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

        getline(&buffer, &buffSize, stdin);   //use get line to get user input, getline reallocs buffer each time its called, no need to free buffer

        log_input(buffer); // read a line of input from user

        if(strcmp(buffer, "\n") == 0){ //if user just enters a blank line
            log_prompt(); // issues another prompt
            fflush(stdout);
            continue;
        }

        //remove the new line from the end of buffer since its causing a new line to be printed before question mark for bogus error
        size_t length = strlen(buffer);
        if(buffer[length - 1] == '\n'){
            buffer[length - 1] = '\0';
        }

        //iterate through the buffer to see what the user input and checkk if its a valid input or not
        //the user input should match the terminal commands

        //split the input based on spaces using strtok
        char* token = strtok(buffer, " ");

        while(token != NULL){

            //compare the strings, if returns 0 its equal
            if(strcmp(token, "help") == 0){
                fprintf(stdout, "Available commands:\n"
                       "help -- Print this help message\n"
                       "quit (<=0 args) -- Quit the program\n"
                       "show (<=1 args) -- Show process info\n"
                       "run (>=1 args) -- Start a process\n"
                       "stop (1 args) -- Stop a running process\n"
                       "cont (1 args) -- Continue a stopped process\n"
                       "release (1 args) -- Stop tracing a process, allowing it to continue normally\n"
                       "wait (1-2 args) -- Wait for a process to enter a specified state\n"
                       "kill (1 args) -- Forcibly terminate a process\n"
                       "peek (2-3 args) -- Read from the address space of a traced process\n"
                       "poke (3 args) -- Write to the address space of a traced process\n"
                       "bt (1 args) -- Show a stack trace for a traced process\n");

                break;
            } 
            else if(strcmp(token, "quit") == 0){

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

            token = strtok(NULL, " "); //get the next token

        }


        log_prompt(); // issues another prompt

        fflush(stdout); //fflush the stdout after every time the user prints something
    }


    return 0;
}

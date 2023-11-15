#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"

int main(int argc, char *argv[]) {

    log_startup(); //starts up deets

    log_prompt(); // issues a prompt

    char* buffer = NULL; //store the user input into a buffer
    size_t buffSize = 0;


    for(;;){ //infinite while loop
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

        //these are booleans to see which command was run by user
        int quitProg = 0, showBool = 0, runBool = 0, stopBool = 0, contBool = 0, releaseBool = 0, waitBool = 0, killBool = 0, peekBool = 0, pokeBool = 0, btBool = 0;

        int argCount = 1; //needs to start at 1 cause we already got a token out


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
                       "bt (1-2 args) -- Show a stack trace for a traced process\n");

                break;
            }
            else if(strcmp(token, "quit") == 0){
                //terminates deet only AFTER killing any processes being debugged
                //has to wait before they actually terminate before exiting itself

                token = strtok(NULL, " "); //get the next token

                if (token != NULL){
                     //QUIT ALLOWS NO ARGUMENTS
                    log_error(buffer); //send error msg with token
                    fprintf(stdout, "?\n");
                    break;
                }
                else{
                    log_shutdown();
                    quitProg = 1;
                    break;
                }


            }
            else if(strcmp(token, "show") == 0 || showBool == 1){
                showBool = 1;
                if(argCount > 2){
                    log_error(buffer); //send error msg with token
                    fprintf(stdout, "?\n");
                    break;
                }
            }
            else if(strcmp(token, "run") == 0 || runBool == 1){
                runBool = 1;
            }
            else if(strcmp(token, "stop") == 0 || stopBool == 1){
                stopBool = 1;
            }
            else if(strcmp(token, "cont") == 0 || contBool == 1){
                contBool = 1;
            }   
            else if(strcmp(token, "release") == 0 || releaseBool == 1){
                releaseBool = 1;
            }
            else if(strcmp(token, "wait") == 0 || waitBool == 1){
                waitBool = 1;
            }
            else if(strcmp(token, "kill") == 0 || killBool == 1){
                killBool = 1;
            }
            else if(strcmp(token, "peek") == 0 || peekBool == 1){
                peekBool = 1;
            }
            else if(strcmp(token, "poke") == 0 || pokeBool == 1){
                pokeBool = 1;
            }
            else if(strcmp(token, "bt") == 0 || btBool == 1){
                btBool = 1;
            }
            else{
                //IF None of the commands match the user input, its an error
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");

                //whenever you get an error always log the error and then question mark

            }

            token = strtok(NULL, " "); //get the next token

            argCount++; //count the num of arguments

            if(runBool == 1 && argCount == 2 && token == NULL){ //if run has no arguments, it is an error
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                break;
            }

            if(stopBool == 1){ //if stop has no arguments, error
                if((argCount == 2 && token == NULL) || (argCount == 3 && token != NULL)){ //if stop has no arguments, or more than 1 argument
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                break;
                }
            }
            if(contBool == 1){ //if stop has no arguments, error
                if((argCount == 2 && token == NULL) || (argCount == 3 && token != NULL)){ //if stop has no arguments, or more than 1 argument
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                break;
                }
            }
            if(releaseBool == 1){ //if stop has no arguments, error
                if((argCount == 2 && token == NULL) || (argCount == 3 && token != NULL)){ //if stop has no arguments, or more than 1 argument
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                break;
                }
            }
            if(killBool == 1){ //if stop has no arguments, error
                if((argCount == 2 && token == NULL) || (argCount == 3 && token != NULL)){ //if stop has no arguments, or more than 1 argument
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                break;
                }
            }
           

        } //end of the infinite while loop

        if(quitProg == 1){
            break; //if the user typed 'quit' just end
        }

        log_prompt(); // issues another prompt

        fflush(stdout); //fflush the stdout after every time the user prints something
    }


    return 0;
}

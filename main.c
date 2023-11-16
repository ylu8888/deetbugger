#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>


int main(int argc, char *argv[]) {

    log_startup(); //starts up deets

    log_prompt(); // issues a prompt
    fprintf(stdout, "deet> ");

    char* buffer = NULL; //store the user input into a buffer
    size_t buffSize = 0;

    //int deetCount = 0;

    for(;;){ //infinite while loop
       // fprintf(stdout, "deet> ");

        getline(&buffer, &buffSize, stdin);   //use get line to get user input, getline reallocs buffer each time its called, no need to free buffer

        log_input(buffer); // read a line of input from user

        if(strcmp(buffer, "\n") == 0){ //if user just enters a blank line
            log_prompt(); // issues another prompt
            fprintf(stdout, "deet> ");
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

        char *tempBuff = strdup(buffer);  //COPY buffer temporarily into another one, need it for Printing since strTok changes buffer

        char* token = strtok(buffer, " "); //split the input based on spaces using strtok

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

                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
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
                    log_prompt(); // issues another prompt
                    fprintf(stdout, "deet> ");
                    fflush(stdout); 
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
                    log_prompt(); // issues another prompt
                    fprintf(stdout, "deet> ");
                    fflush(stdout); 
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
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
                //whenever you get an error always log the error and then question mark

            }

            token = strtok(NULL, " "); //get the next token

            argCount++; //count the num of arguments

            if(runBool == 1 && argCount == 2 && token == NULL){ //if run has no arguments, it is an error
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
            }

            if(stopBool == 1 || contBool == 1 || releaseBool == 1 || killBool == 1){ //if stop, cont, release, kill have no arguments, or more than 1 argument
                if((argCount == 2 && token == NULL) || (argCount == 3 && token != NULL)){ 
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
                }
            }
            if(waitBool == 1 || btBool == 1){ //if wait has no arguments, or more than 2 argument
                if((argCount == 2 && token == NULL) || (argCount == 4 && token != NULL)){
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
                }
            }
            if(peekBool == 1){
                if((argCount == 2 && token == NULL) || (argCount == 3 && token == NULL) || (argCount == 5 && token != NULL)){ //if peekBool has no arguments, or only 1, or more than 3
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
                }
            }
            if(pokeBool == 1){
                if((argCount == 2 && token == NULL) || (argCount == 3 && token == NULL) || (argCount == 4 && token == NULL) || (argCount == 5 && token != NULL)){ //if wait has no arguments, or less than 3, or more than 3 argument
                log_error(buffer); //send error msg with token
                fprintf(stdout, "?\n");
                log_prompt(); // issues another prompt
                fprintf(stdout, "deet> ");
                fflush(stdout); 
                break;
                }
            }

            char* progName;
            if(runBool == 1 && argCount == 2){ //GRAB THE name of the program AFTER 'run' for ex: grabbing echo from run echo a b c 
                strcopy(token, progName); //copy echo into progName
            }

            if(runBool == 1 && token == NULL){ //only if run arguments are valid
               pid_t p = fork(); 
                //int p = 1;

                if(p == 0){ //child process has been created
                    dup2(2, 1); //close stdout and redirect to stderr
                    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

                    execvp(progName, 
                    //send in echo as first one
                    // array with a b c
                    //null as the argv
                    //exec argument is argv

                    if(argCount
                    
                }
                if(p > 0){ //it returned to parent 
                    
                      log_state_change(p, PSTATE_NONE, PSTATE_RUNNING, 999); //only log state changes in the parent, 999 is my own status number

                      fprintf(stdout, "0\t");
                      fprintf(stdout, "%d", p);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "T\t");
                      fprintf(stdout, "running\t");
                      fprintf(stdout, "%s\n", tempBuff);

                      log_prompt(); // issues another prompt
                     fprintf(stdout, "deet> ");
                     fflush(stdout); 

                     // waitpid

                }
                // else if(p < 0){ //unsuccessful, did not create child process and send error
                //     log_error(buffer); //send error msg with token
                //     fprintf(stdout, "?\n");
                //     break;
                // }
           }
           

        } //end of the infinite while loop

        free(buffer);

        if(quitProg == 1){
            break; //if the user typed 'quit' just end
        }

        // log_prompt(); // issues another prompt

        //fflush(stdout); //fflush the stdout after every time the user prints something

        //DO RUN first after help
        //the way run works is that you have a main loop that continuously loops and prints deet until next user input
        //if user types run, it needs to split itself into 2 processes
        //one continues the main while loop with prompt deet
        //2nd one becomes the child process
        //takes in a bunch of strings and those are copied into a new command 
        //if you went into command prompt without running deet, and run echo abc
        //echo is a built in command that prints whatever comes after it
        //echo abc will print abc into terminal
        //sleep is another command that makes the terminal sleep for a couple seconds
        //convert a process to another process, using execcvp()
        //dup2, ptrace, and then execcvp in that order exactly
        
        //Signal stuff:
        //fork you have a process A and fork just forks the processs
        //it basically creates a child process
        //fork is a method that you can call inside your program
        //its a method inside of package
        //search the man pages (man fork) to see documentation
        //fork takes in nothing and outputs a pid_t
        //you fork it, get a pid (process id) if the pid (need to check what it can return and then condition on all of those)
        //if pid == 0, then its a child process otherwise if less than 0,
        //if pid > 0 then its a parent process 
        //the moment u fork, the process input creates a child input, and the child does whatever sleep/echo/sample prog is suppose to do
        // DUP2 is basically redirecting input (read man pages)
        //redirect stdout/stderr (make sure you test it by redirecting input and see if it goes to right place)
        // ptrace() next time you call execcvp() or any exec command, hand control over to parent process
        //ptrace has alot of commands, the important one is traceMe()
        //its from a child process, the child process tells the parent process "trace me"
        //the next time execcvp it hands over control to the parent process
        //by handing over control, it means the parent will get a signal that tells it that somethign interseting has happened
        //and the child process will stop itself and the parent will be notified that the child has stopped
        //once the child process is done, you can do stuff now like in the debugger
        //execvp() he talks aobut it in class and how they dont return
        //dont do anything after execcvp gets called
        //just turn the process into something else and it cant run any commands that you put directly underneath it
        //literally just tell execcvp to turn itself into an echo command
        //it takes in strings and you just tell it 'turn into echo' with all the arguments behind it

    }


    return 0;
}

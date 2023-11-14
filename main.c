#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"

int main(int argc, char *argv[]) {

    log_startup(); //starts up deets

    log_prompt(); // issues a prompt

    char* buffer = NULL; //store the user input into a buffer
    size_t buffSize = 0;

    int quitProg = 0;

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

        //int argCount = 0;

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
                //terminates deet only AFTER killing any processes being debugged
                //has to wait before they actually terminate before exiting itself

                log_shutdown();
                quitProg = 1;
                break;
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

        } //end of the infinite while loop

        if(quitProg == 1){
            break; //if the user typed 'quit' just end
        }

        log_prompt(); // issues another prompt

        fflush(stdout); //fflush the stdout after every time the user prints something

        //DO RUN first after help
        //the way run works is that you have a main loop that continuously loops and prints deep until next user input
        //if user types run, it needs to split itself into 2 processes
        //one continues the main while loop with prompt deet
        //2nd one becomes the child process
        //takes in a bunch of strings and those are copied into a new command 
        //if you went itno command prompt without running deet, and run echo abc
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

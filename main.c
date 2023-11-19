#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include "helper.h"

int main(int argc, char *argv[]) {
    log_startup(); //starts up deets
    log_prompt(); // issues a prompt
    fprintf(stdout, "deet> ");

    char* buffer = NULL; //store the user input into a buffer
    char* tempBuff = NULL;
    char* toadBuff = NULL;
    char* frogBuff = NULL;
    size_t buffSize = 0;

    struct sigaction sigact;
    sigact.sa_handler = sig_handler; //assign signal handler function to the sa_handler field of the sigact struct
    sigact.sa_flags = SA_RESTART; //system call is restarted when signal handler returns
    //set up a signal handler for the SIGCHLD signal using sigaction
    
       if(sigaction(SIGCHLD, &sigact, NULL) == -1){
            log_error(buffer); //send error msg with token
            fprintf(stdout, "?\n");
            log_prompt(); // issues another prompt
            fprintf(stdout, "deet> ");
         
        }; //check the return and if condition it 

    PROCESS* procArray[100];//make an array of struct PROCESSES to store the users processes
    for (int i = 0; i < 100; ++i) {
        procArray[i] = (PROCESS*)malloc(sizeof(PROCESS)); //allocate memory for 100 structs
    }
    int numProc = 0; //count the number of processes the user makes

    pid_t p = 0; //makke pid global so can access everywhere


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

        tempBuff = strdup(buffer);  //COPY buffer temporarily into another one, need it for Printing since strTok changes buffer
        toadBuff = strdup(buffer);

        char* spaceIdx = strchr(tempBuff, ' ');
        if(spaceIdx != NULL){
            strcpy(toadBuff, spaceIdx + 1);
        }

        frogBuff = strdup(buffer); //ill just use this to count the arguments 

        char* frogTok = strtok(frogBuff, " "); //using this just to count the args
        int frogCount = 0;
        while(frogTok != NULL){
            frogCount++;
            frogTok = strtok(NULL, " ");
            //fprintf(stdout, "%d", frogCount);
        }
        frogCount = frogCount - 2; //subtract the 'run' and the 'echo' aka progName to get the count of the 

        char* token = strtok(buffer, " "); //split the input based on spaces using strtok

        //these are booleans to see which command was run by user
        int quitProg = 0, showBool = 0, runBool = 0, stopBool = 0, contBool = 0, releaseBool = 0, waitBool = 0, killBool = 0, peekBool = 0, pokeBool = 0, btBool = 0;
        int argCount = 1; //needs to start at 1 cause we already got a token out
        int argNum = 0; //for getting 'show 1'
        char* progName = NULL;
        char *arguv[frogCount + 1];

        //iterate through the buffer to see what the user input and checkk if its a valid input or not
        //the user input should match the terminal commands
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
                    // log_shutdown();
                     quitProg = 1;
                    // break;
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
                //when implementing the kill functionality, loop through the array of structs 
                //and set that process equal to NULL since we are killing it
                //then whenever a new child process is created we loop through array again
                //and the first NULL we encounter we store the process in that array element
                //actually only do htis when the user calls the RUN command again ^^^ 

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

            //ARGUMENT COUNT ERROR CHECKER
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
            } //END OF ARGUMENT COUNT ERROR CHECKING

            //START OF RUN COMMAND 
            
            if(runBool == 1 && argCount == 2){ //GRAB THE name of the program AFTER 'run' for ex: grabbing echo from run echo a b c 
                progName = strdup(token);//copy echo into progName as the first one

            }

            if(argCount > 2){ //only want to grab the arguments after echo aka progName
                arguv[argCount - 3] = token;
                //fprintf(stdout, "%s\n", arguv[argCount - 3]); //ITS WORKING
            }
            arguv[frogCount] = NULL; //set the last element of argv to NULL as per execvp

            //fprintf(stdout, "%d", frogCount);

            if(runBool == 1 && token == NULL){ //only if run arguments are valid
               p = fork(); 

                if(p == 0){ //child process has been created
                    numProc++; //keep track of the number of child processes being created
                    dup2(2, 1); //close stdout and redirect to stderr
                    ptrace(PTRACE_TRACEME, 0, NULL, NULL);  //ptrace will call sigstop and the parent will get a SIGCHLD

                    int execRes = execvp(progName, arguv); //arguv is array with [a, b, c, NULL]
                    if(execRes == -1){ //if execvp returns -1 as error, just log to terminal
                        // log_error(buffer); //send error msg with token
                        // fprintf(stdout, "?\n");
                        // log_prompt(); // issues another prompt
                        // fprintf(stdout, "deet> ");
                        // fflush(stdout); 
                        //perror("execvp");
                        
                      //  fprintf(stdout, "IM IN THE EXECRES");
                        exit(EXIT_SUCCESS);

                    }
                }
                if(p > 0){ //it returned to parent 
                      log_state_change(p, PSTATE_NONE, PSTATE_RUNNING, 999); //only log state changes in the parent, 999 is my own status number

                    //PRINT out the status line for when the process STOPS

                    //To get the deetID
                    //loop thru the array
                    //looking for child process id
                    //look for index where child process occurs
                    
                    for(int i = 0; i < 100; i++){ //loop through the struct array
                       //fprintf(stdout, "%d\n", procArray[i]->pid); //defaulted on zero 0
                       //fprintf(stdout, "%c\t", procArray[i]->trace); //defaulted on empty space ' '
                       //fprintf(stdout, "%s\t", procArray[i]->state); //defaulted on NULL
                      // fprintf(stdout, "\t");
                      // fprintf(stdout, "%s\n", procArray[i]->args); //defaulted on NULL
                      // fprintf(stdout, "%d\n", i);

                        if((procArray[i] != NULL) || strcmp(procArray[i]->state, "dead") == 0){ //the first array index where its not NULL like its not even initialized yet
                            //OR if the state of the proc array is dead, then we replace it
                            if(procArray[i]->pid == 0){ //ONLY IF THE PROCARRAY IS NOT DEFAULTED ON NULL
                            procArray[i]->pid = p;
                            procArray[i]->trace = 'T';
                            procArray[i]->state = "stopped";
                            procArray[i]->args = toadBuff;
                           // procArray[i].exit //we can just leave this alone since we dont need to touch .exit unless its kill or dead

                            break;
                            }
                        }
                    }

                    for(int i = 0; i < 100; i++){
                        if(procArray[i]->pid == 0) break; //IF WE RUN INTO A NULL INDEX IN ARRAY
                        if(procArray[i]->pid == p){
                             fprintf(stdout, "%d\t", i);
                            break;
                        }
                    }
                    fprintf(stdout, "%d", p);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "T\t");
                      fprintf(stdout, "running\t");
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\n", toadBuff);

                      log_prompt(); // issues another prompt

                  // log_state_change(p, PSTATE_RUNNING, PSTATE_STOPPED, 999); //already being done in the signal handler func

                    //the PTRACEME will send a SIGCHLD which gets caught by the signal handler function 
                    //which logs the signal 17 and then the parent function will print this status line below
                      for(int i = 0; i < 100; i++){
                        if(procArray[i]->pid == 0) break; //IF WE RUN INTO A NULL INDEX IN ARRAY
                        if(procArray[i]->pid == p){
                             fprintf(stdout, "%d\t", i);
                            break;
                        }
                     }

                      fprintf(stdout, "%d", p);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "T\t");
                      fprintf(stdout, "stopped\t");
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\n", toadBuff);

                      // && procArray[i]->trace != '\0'

                     log_prompt();
                     fprintf(stdout, "deet> ");
                     fflush(stdout); 

                }
                if(p < 0){ //unsuccessful, did not create child process and send error
                    log_error(buffer); //send error msg with token
                    fprintf(stdout, "?\n");
                    log_prompt(); // issues another prompt
                    fprintf(stdout, "deet> ");
                    fflush(stdout); 
                    break;
                }
           }// END OF THE RUN COMMAND

            //go throu array if u see a dead process
            //if u want to put a new process in the spot of the dead process
            //you would have to replace its spot
            //when printing it out, you have to log state dead to none
            //and another log state of none to run

            if(showBool == 1){ //START OF THE SHOW COMMAND
                int emptyProcBool = 0;
                
                if(token != NULL){
                    argNum = atoi(token); //get the STRING TO NUM from the argument, for ex "show 1" processes the '1' string and converts to int 1
                }
                if(argCount == 2 && token == NULL){ //if there no specified process
                    for(int i = 0; i < 100; i++){ //loop through the struct array
                     if(procArray[i]->pid == 0) break; //when we run into a NULL index in our struct array 
                     emptyProcBool = 1;
                      fprintf(stdout, "%d\t", i);
                      fprintf(stdout, "%d\t", procArray[i]->pid);
                      fprintf(stdout, "%c\t", procArray[i]->trace);
                      fprintf(stdout, "%s", procArray[i]->state);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\n", procArray[i]->args);
                     
                    }
                    if(emptyProcBool == 0){ //if the proc array is empty and the user types show, then error
                        log_error(buffer); //send error msg with token
                        fprintf(stdout, "?\n");
                        log_prompt(); // issues another prompt
                        fprintf(stdout, "deet> ");
                        fflush(stdout); 
                        break;
                    }
                     log_prompt();
                     fprintf(stdout, "deet> ");
                     fflush(stdout); 
                    
                }
                else if(argCount == 3 && token == NULL){ // if theres a specified process
                    //fprintf(stdout, "MADE IT HERE");
                   // fprintf(stdout, "%d", argNum);
                   // JUST PRINT OUT THE PROCESS FROM PROCARRAY AT THAT ARGUMENT INDEX
                    if(procArray[argNum]->pid != 0){ //only print this if the show 'argNum ' is VALID
                      
                      fprintf(stdout, "%d\t", argNum);
                      fprintf(stdout, "%d\t", procArray[argNum]->pid);
                      fprintf(stdout, "%c\t", procArray[argNum]->trace);
                      fprintf(stdout, "%s\t", procArray[argNum]->state);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\n", procArray[argNum]->args);

                      log_prompt();
                     fprintf(stdout, "deet> ");
                     fflush(stdout); 
                  } else { //if it does show 3 and the 3rd index is NULL, then return errs
                    log_error(buffer); //send error msg with token
                    fprintf(stdout, "?\n");
                    log_prompt(); // issues another prompt
                    fprintf(stdout, "deet> ");
                    fflush(stdout); 
                    break;
                  }
                    
                }
            } //END OF THE SHOW COMMAND 

            if(quitProg == 1){ //START OF QUIT COMMAND
                for(int i = 0; i < 100; i++){
                   if(procArray[i]->pid == 0) break; //when we run into a NULL index in our struct array 
                    procArray[i]->state = "killed";
                  // log_state_change(procArray[i]->pid, PSTATE_STOPPED, PSTATE_KILLED, 999); //log state from stopped to killed

                    //send a kill signal to kill the process
                    int killRes = kill(procArray[i]->pid, SIGKILL);
                    if(killRes == 0){
                      // fprintf(stdout, "MADE IT INTO KILL");
                        log_state_change(procArray[i]->pid, PSTATE_STOPPED, PSTATE_KILLED, 999);
                    } else if(killRes != 0){
                        log_error(buffer); //send error msg with token
                        fprintf(stdout, "?\n");
                        log_prompt(); // issues another prompt
                        fprintf(stdout, "deet> ");
                        fflush(stdout); 
                        break;
                    
                    }
                   
                }

                for(int i = 0; i < 100; i++){
                    if(procArray[i]->pid == 0) break;
                    fprintf(stdout, "%d\t", i);
                      fprintf(stdout, "%d\t", procArray[i]->pid);
                      fprintf(stdout, "%c\t", procArray[i]->trace);
                      fprintf(stdout, "%s", procArray[i]->state);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\n", procArray[i]->args);
                }
                //sigchild, pid, sigkill

                for(int i = 0; i < 100; i++){
                   if(procArray[i]->pid == 0) break; //when we run into a NULL index in our struct array 
                    //fprintf(stdout, "DO I EVER MAKE IT THIS FOR LOOP");
                    int status;
                    waitpid(procArray[i]->pid, &status, 0); //wait for the child processes to terminate 
                    procArray[i]->state = "dead";
                    log_state_change(procArray[i]->pid, PSTATE_KILLED, PSTATE_DEAD, 999); //log state from killed to dead
                    
                
                }

                for(int i = 0; i < 100; i++){
                    if(procArray[i]->pid == 0) break;

                      fprintf(stdout, "%d\t", i);
                      fprintf(stdout, "%d\t", procArray[i]->pid);
                      fprintf(stdout, "%c\t", procArray[i]->trace);
                      fprintf(stdout, "%s", procArray[i]->state);
                      fprintf(stdout, "\t");
                      fprintf(stdout, "%s\t", "0x9");
                      fprintf(stdout, "%s\n", procArray[i]->args);
                }

                // log_shutdown();
                // exit(0);
                break;
                //once user hits control c, you get the sigint
                //loop thru the array and send a kill signal to kill all the processes
                //then quit and shutdown
            } //END OF QUIT COMMAND

            if(contBool == 1){
                //kill then get the pid, then send a ptraceCONT

                if(token != NULL){
                    argNum = atoi(token); //get the STRING TO NUM from the argument, for ex "show 1" processes the '1' string and converts to int 1
                }

                if(procArray[argNum]->pid == 0){  //IF THE ARGUMENT IS INVALID
                    log_error(buffer); //send error msg with token
                    fprintf(stdout, "?\n");
                    log_prompt(); // issues another prompt
                    fprintf(stdout, "deet> ");
                    fflush(stdout); 
                    break;
                }
                

                //will never go inside WIFCONTINUED
                if (ptrace(PTRACE_CONT, p, NULL, NULL) == 0){
                    perror("ptrace");
                    // Handle the error as needed
                }
            }

        } //end of the infinite while loop

        if(quitProg == 1){
            log_shutdown();
            exit(0);
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

    free(buffer);
    free(tempBuff);
    free(frogBuff);
    free(toadBuff);

     for (int i = 0; i < 100; ++i) {
            free(procArray[i]); //free the proc array inviidually each element
            // free(procArray[i]->state);
            // free(procArray[i]->args);

    }

    return 0;
}

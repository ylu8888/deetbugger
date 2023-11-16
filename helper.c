#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "deet.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>

void sig_handler(int signum){
    //when u do sigaction, it calls these handlers
    int status;
    pid_t p;
   
    while((p = waitpid(0, &status, WUNTRACED)) > 0){ //loop that continues waiting for child processes until there are none left
        //waits for child process to change state and when child process is detected, check to see if it has STOPPED
        //if stopped, then log the signal and then change state from running to stopped
        if(WIFSTOPPED(status)){
            log_signal(signum); //This function must be called as the first action in the signal handler
            log_state_change(p, PSTATE_RUNNING, PSTATE_STOPPED, 999);
            break;
        }
        else if(WIFEXITED(status)){
            
        }
        else if(WIFCONTINUED(status)){
            
        }
        else if(WIFSIGNALED(status)){
            
        }
    }
}

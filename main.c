#include <stdlib.h>
#include <stdio.h>
#include "deet.h"

int main(int argc, char *argv[]) {

    log_startup(); //starts up deets

    log_prompt(); // issues a prompt

    fprintf(stdout, "deet> ");

    char* buffer = NULL; //store the user input into a buffer
    size_t buffSize = 0;

    getline(&buffer,&buffSize,stdin);   //use get line to get user input

    log_input(buffer); // read a line of input from user

    log_prompt(); // issues another prompt

    fflush(stdout); //fflush the stdout after every time the user prints something


    return 0;
}

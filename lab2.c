//  lab2.c
//
//  ICS 53 Spring 2015
//  Lab 2: A Simple Shell

#include <stdio.h>
#include "csapp.h"

#define LINES   25
#define CHARS   80

int main(int argc, const char * argv[])
{
    // char screen[LINES][CHARS];
    char input[CHARS];
    while(1)
    {
        printf("prompt> ");
        memset(input, CHARS, '\0');
        fgets(input, CHARS, stdin);
        fputs(input, stdout);
        
        // Testing code for catching stderr with blank line 
        /* if (input[0] == '\n')
        {
            fputs("Blank line\n", stderr);
            //fprintf(stderr, "Blank line\n");
        } */
        
        if (input[0] == 'q' && input[1] == 'u' && input[2] == 'i' && input[3] == 't')
        {
            // clear memory before exit(0)
            memset(input, CHARS, '\0');
            exit(0);
        }
        // Do something
    }
    // No code can be running after while loop
    // Since using exit(0)
    return 0;
}

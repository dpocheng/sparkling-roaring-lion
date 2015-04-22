/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

int main() 
{
    char cmdline[MAXLINE]; /* Command line */
    
    while (1) {
        /* Read */
        printf("prompt> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin)) {
            exit(0);
        }
        
        
        /* Evaluate */
        eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    int ip, op;
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL) {
	 return;   /* Ignore empty lines */
    }

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            
            if (execve(argv[0], argv, environ) < 0) {
                    printf("%s: Command not found.\n", argv[0]);
                    exit(0);
            }
            /*if (execve(argv[1], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[1]);
                exit(0);
            }*/
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                unix_error("waitfg: waitpid error");
            }
        }
        else {
            printf("%d %s", pid, cmdline);
            
            // WNOHANG|WUNTRACED: Return immediately, with a return value of 0, if
            // the child process has stopped or terminated, or with a return value
            // equal to the child process pid if the child process stopped or terminated
            int childStatus;
            waitpid(pid, &childStatus, WNOHANG|WUNTRACED);
        }
        
        /*if(*argv[1] == '<') {
            //dup2(ip, op);
            //read(ip, cmdline, 1);
            printf("testing\n");
        }
        else if (*argv[1] == '>') {
            
        }*/
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    /* quit command */
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }
    /* Ignore singleton & */
    if (!strcmp(argv[0], "&")) {
        return 1;
    }
    /* Not a builtin command */
    return 0;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    
    while (*buf && (*buf == ' ')) {
        buf++;           /* Ignore leading spaces */
    }

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) {
	       buf++;       /* Ignore spaces */
        }
    }
    argv[argc] = NULL;
    
    if (argc == 0) {
        return 1;       /* Ignore blank line */
    }

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
        argv[--argc] = NULL;
    }
    return bg;
}
/* $end parseline */



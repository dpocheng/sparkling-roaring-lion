#include "csapp.h"
#define MAXARGS   128

/* function prototypes */
void sigchldHandler(int sig);
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

/* $begin sigchldHandler */
/* sigchldHandler - Handles the SIGCHLD signal from child process sent to the parent
 *                  process to properly reap the processes that run in the background */
void sigchldHandler(int sig) {
    pid_t pid;
    /* -1   --> waitset consists of all of the parent's child processes
     * NULL --> discard the returned status from the child process that is reaped
     * 0    --> default, suspends the execution of the calling process until a child
     *          process in its waitset terminates. (In this case we are calling Waitpid
     *          only after a child in the waitset has terminated, and therefore will not
     *          block execution) */
    while((pid = Waitpid(-1, NULL, 0)) > 0) {
        switch (errno) {
            case EINTR:
                continue;

            case ECHILD:
                continue;
        }
        return;
    }
}
/* $end sigchldHandler */

/* $begin shellmain */
int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    // Register the Signal handler to reap child processes in the foreground and the background
    Signal(SIGCHLD, sigchldHandler);

    while (1)
    {
        /* Read */
        printf("prompt> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
        {
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
    char *argv[MAXARGS]; /* Argument list Execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    char redirectBuf[MAXLINE];
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    int argc;
    int in, out;
    int redirectIO = 0;  /* A boolean to save if redirection has been detected */
    int current_input, current_output;
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    
    if (argv[0] == NULL)
    {
        return;   /* Ignore empty lines */
    }

    if (!builtin_command(argv)) {
        pid = Fork();     /* This allows for non-blocking of slow system calls */

        /* Child runs user job */
        if (pid == 0) {
            // Store stdin & stdout file handles
            current_input = dup(0);
            current_output = dup(1);

            while (argv[argc] != '\0') {
                // Redirect input and output if '<' or '>' found
                if (*argv[argc] == '<' || *argv[argc] == '>')
                {
                    if (*argv[argc] == '<' && argv[argc+1] != '\0')
                    {
                        in = Open(argv[argc+1], O_RDONLY, S_IRUSR | S_IXUSR);
                        Dup2(in, 0);
                        read(in, redirectBuf, MAXBUF);
                        Close(in);
                        strcpy(buf, redirectBuf);
                    }
                    if (*argv[argc] == '>' && argv[argc+1] != '\0')
                    {
                        out = Open(argv[argc+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                        Dup2(out, 1);
                        Close(out);
                    }
                }
                argc++;
            }
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }


        /* Parent waits for foreground job to terminate */
        if (!bg) {
            /* Block this process until sigchldHandler returns */
            Pause();
        }
        else {
            printf("%d %s", pid, cmdline);
            /* Reap the child processes in the background */
        }
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
#include <ctype.h>
#include <stdio.h>
#include "lab.h"

char *get_prompt(const char *env)
{
    const char* prompt = getenv(env); // returns NULL if env variable does not exist
    if(prompt) return prompt;
    return "shell>";
}

int change_dir(char **dir)
{

}

// Form needed by execvp => int execvp(const char *file, char *const argv[]);
char **cmd_parse(char const *line)
{
    if(*line == NULL) exit(EXIT_SUCCESS); // Check for EOF input

    char** argv = malloc(2 * sizeof(char*)); // Allocate space for arguments
    if(!argv)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Split line by spaces and store in argv
    printf("Tokenizing...\n");
    int position = 0;
    char* token = strtok(line, " ");
    while(token != NULL)
    {
        argv[position] = token;
        position++;
        token = strtok(NULL, " ");
    }
    printf("Successfully tokenized!\n");
    printf("Tokens: %s %s\n", argv[0], argv[1]);
    return argv;
}

void cmd_free(char** line)
{
    for(int i=0; line[i]; i++)
        free(line[i]);
    free(line);
}

char *trim_white(char *line)
{
    // Allocate new string to copy the line
    char* line_copy = malloc(sizeof(char*));
    if(!line_copy)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    // Find index of the first non-whitespace char
    int start;
    for(start = 0; start < strlen(line); start++)
    {
        if(isspace((unsigned char)line[start])) 
            continue;
        else break;
    }

    // Find index of the last non-whitespace char
    int end;
    for(end = strlen(line) - 1; end >= 0; end--)
    {
        if(isspace((unsigned char)line[end])) 
            continue;
        else break;
    }

    // Copy all the in-between chars to the new string
    int mid;
    int pos = 0;
    for(mid = start; mid <= end; mid++) {
        line_copy[pos] = line[mid];
        pos++;
    }
    line_copy[pos++] = '\0'; // Null terminate the string
    
    free(line);
    return line_copy;
}

bool do_builtin(struct shell* sh, char** argv)
{
    // Do "exit" cmd
    if(strcmp(argv[0], "exit") == 0)
    {
        // cmd_free(argv);
        // free(sh->prompt);
        exit(EXIT_SUCCESS);
        return true;
    }

    // Do "cd" cmd
    else if(strcmp(*argv, "cd") == 0)
    {
        char* dir = *(argv + 1);
        printf("target: %s", *(argv + 1));

        // No second arg provided => cd to home dir
        if(*dir == NULL)
        {   
            if(dir = getenv("HOME")) // Try to retrieve home dir from env variable
            {
                printf("cd'd to ~");
                chdir(dir); 
            }
            else if((dir = getpwuid(getuid())->pw_dir)) // Try to retrieve home dir from user id
            {
                printf("cd'd to ~");
                chdir(dir);
            }
            else perror("cd");
        }

        // Second arg provided => switch to the specified dir
        else
        {
            if(chdir(dir) == -1)
                perror("cd");
            else printf("successfully switched to: %s", *dir);
        }
        
        return true;
    }
    // TODO: Check for other built-ins

    return false;
}

void sh_init(struct shell *sh)
{
    /* See if we are running interactively.  */
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);
    sh->prompt = get_prompt("MY_PROMPT");

    if(sh->shell_is_interactive)
    {
        /* Loop until we are in the foreground.  */
        while(tcgetpgrp(sh->shell_terminal) != (sh->shell_pgid = getpgrp()))
            kill(- sh->shell_pgid, SIGTTIN);

        /* Ignore interactive and job-control signals.  */
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        // signal(SIGCHLD, SIG_IGN);

        /* Put ourselves in our own process group.  */
        sh->shell_pgid = getpid();
        if(setpgid(sh->shell_pgid, sh->shell_pgid) < 0)
        {
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp(sh->shell_terminal, sh->shell_pgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr(sh->shell_terminal, &(sh->shell_tmodes));
    }
}

void sh_destroy(struct shell *sh)
{

}


// "args" are the flags passed when calling the shell, i.e. -v and -h
// args are also flags passed to commands inside the shell, like ls -l
void parse_args(int argc, char **argv)
{
    int opt;

    while((opt = getopt(argc, argv, SHELL_ARGS)) != -1)
        switch(opt)
        {
            case 'v':
                printf("***********\n * Shell * \n***********\nVersion: %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(EXIT_SUCCESS);
                break;
            case '?':
                if(isprint(optopt))
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else
                {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
            default:
                abort();
        }

    // printf ("vflag = %d\n", vflag);
    for(int index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);
}

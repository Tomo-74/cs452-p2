#include <ctype.h>
#include <stdio.h>
#include "lab.h"

char *get_prompt(const char *env)
{

}

int change_dir(char **dir)
{

}

char **cmd_parse(char const *line)
{

}

void cmd_free(char ** line)
{

}

char *trim_white(char *line)
{

}

bool do_builtin(struct shell *sh, char **argv)
{

}

void sh_init(struct shell *sh)
{
    /* See if we are running interactively.  */
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);

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
    {
        switch(opt)
        {
            case 'v':
                printf("***********\n * Shell * \n***********\nVersion: %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(EXIT_SUCCESS);
                break;
            case '?':
                // if(optopt == 'c')
                //     fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                if(isprint(optopt))
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                    // abort();
                }
                else
                {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                    // abort();
                }
            default:
                abort();
        }
    }

    // printf ("vflag = %d\n", vflag);

    for(int index = optind; index < argc; index++)
    {
        printf ("Non-option argument %s\n", argv[index]);
    }
}

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define RSHELL_RL_BUFSIZE 1024

// exit command
static const char* CMD_EXIT = "exit";

char *rshell_read_line(void)
{
    int bufsize = RSHELL_RL_BUFSIZE;
    int position = 0;
    char *buffer = (char *) malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
       fprintf(stderr, "rhsell: allocation error\n");
       exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        // if we have exceeded the buffer, reallocate
        if (position >= bufsize) {
           bufsize += RSHELL_RL_BUFSIZE;
           buffer = (char *) realloc(buffer, bufsize);
           if (!buffer) {
               fprintf(stderr, "rshell: allocation error\n");
               exit(EXIT_FAILURE);
           }
        }
    }
}

char **rshell_parse_cmd(char *cmd)
{
    int bufsize = RSHELL_RL_BUFSIZE;
    int position = 0;
    char **tokens = (char **) malloc(bufsize * sizeof(char *));
    char *token;
    char *saveptr;
   
    if (!tokens) {
        fprintf(stderr, "rshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(cmd, " ", &saveptr);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += RSHELL_RL_BUFSIZE;
            tokens = (char **) realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "rshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok_r(NULL, " ", &saveptr);
    }
    tokens[position] = NULL; 

    return tokens;
}

int rshell_exec_cmd(char *cmd)
{
    pid_t pid;
    int status;
    char **args = rshell_parse_cmd(cmd);

    pid = fork();
    if (pid == 0) {
        // We are in the child process
         if (execvp(args[0], args) == -1) {
             perror("rshell");
         }
         exit(1);
    } else if (pid < 0) {
        // error forking
        perror("rshell");
        exit(1);
    } else {
        // parent process
        wait(&status);
    }

    free(args);
    return 1;
}

void rshell_exec_cmd_line(char *line)
{
    int bufsize = RSHELL_RL_BUFSIZE;
    int position = 0;
    char *token; 
    char *saveptr;
    char *cmd;

    token = strtok_r(line, ";", &saveptr);
    while (token != NULL) {
        cmd = strdup(token);
        rshell_exec_cmd(cmd); 
        token = strtok_r(NULL, ";", &saveptr);
        free(cmd);
    }
}

void rshell_loop(void)
{
    char *line;
    int status;

    do {
        printf("$ ");
        line = rshell_read_line();
        if (strcmp(line, CMD_EXIT) != 0) {
            rshell_exec_cmd_line(line);
        } else {
            free(line);
            return;
        }

        free(line);
    } while (1);
}

int main(int argc, const char **argv)
{
    rshell_loop();
    return EXIT_SUCCESS;
}

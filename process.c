#include "process.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
int shell_exec_cmd(char **argv) {
    execvp(argv[0], argv);

    int exit_code = 1;

    switch (errno) {
        case EACCES:
            exit_code = 126;
            fprintf(stderr, "%s: Permission denied.\n", argv[0]);
            break;
        case ENOENT:
            exit_code = 127;
            fprintf(stderr, "%s: Command not found.\n", argv[0]);
            break;
    }
    return exit_code;
}

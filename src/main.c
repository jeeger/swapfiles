#define _GNU_SOURCE

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <argp.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/fs.h>

#include "config.h"

const char *argp_program_version = PACKAGE_STRING;
const char args_doc[] = "FILE1 FILE2";
const char doc[] = "Swap FILE1 and FILE2 atomically. Files one and two can also be directories (either or both).";
static struct argp_option argp_options[] =
    {
     {"silent", 's', 0, 0, "Don't output anything.", 0},
     {"verbose", 'v', 0, 0, "Produce verbose output.", 0},
     {0}
    };

struct config {
    char *file1;
    char *file2;
    bool silent;
    bool verbose;
};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct config *config = state->input;
    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num == 0) {
            config->file1 = arg;
        } else if (state->arg_num == 1) {
            config->file2 = arg;
        } else {
            argp_error(state, "Only provide two arguments, extra argument '%s' provided.\n", arg);
        }
        break;
    case ARGP_KEY_FINI:
        if (state->arg_num != 2) {
            argp_error(state, "Provide exactly two file arguments.\n");
        }
        return 0;
    case 's':
        config->silent = true;
        if (config->verbose) {
            argp_error(state, "Cannot have both 'silent' and 'verbose'.");
        }
        return 0;
    case 'v':
        config->verbose = true;
        if (config->silent) {
            argp_error(state, "Cannot have both 'silent' and 'verbose'.");
        }
        return 0;
    case ARGP_KEY_INIT:
    case ARGP_KEY_SUCCESS:
    case ARGP_KEY_END:
        return 0;
    case ARGP_KEY_NO_ARGS:
        argp_error(state, "Pass in FILE1 and FILE2 to swap.\n");
        break;
    default:
        argp_error(state, "Unknown key value %d.\n", key);
        break;
    }
    return 0;
}

int renameat2(int olddirfd, const char *oldpath,
              int newdirfd, const char *newpath, unsigned int flags) {
    return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, flags);
}

int main(int argc, char **argv) {
    struct config config = {0};
    static struct argp argp = {argp_options, parse_opt, args_doc, doc};
    if (argp_parse(&argp, argc, argv, 0, 0, &config) != 0) {
        return 1;
    }

    char *file1_realpath = realpath(config.file1, NULL);
    if (!file1_realpath) {
        fprintf(stderr, "Failed to get real path for %s: %s\n", config.file1, strerror(errno));
        return 1;
    }
    char *file2_realpath = realpath(config.file2, NULL);
    if (!file2_realpath) {
        fprintf(stderr, "Failed to get real path for %s: %s\n", config.file2, strerror(errno));
        return 1;
    }

    if (config.verbose) {
        printf("Absolute path file 1: %s.\n", file1_realpath);
        printf("Absolute path file 2: %s.\n", file2_realpath);
    }

    if (!config.silent) {
        printf("Swapping %s and %s.\n", config.file1, config.file2);
    }
    int result = renameat2(0, file1_realpath,
                           0, file2_realpath,
                           RENAME_EXCHANGE);
    free(file1_realpath);
    free(file2_realpath);
    if (result == -1) {
        perror("Failed to rename");
        return 1;
    }
    return 0;
}

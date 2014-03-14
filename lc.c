/*
 * =====================================================================================
 *
 *       Filename:  lc.c
 *
 *    Description:  line count
 *
 *        Version:  1.0
 *        Created:  2014年01月08日 22时10分31秒
 *       Compiler:  gcc
 *
 *         Author:  codeplayer, silencly07@gmail.com
 *      Copyright:  Copyright (c) 2014, codeplayer.org
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define MAXLINE 4096

static int total_lines;

void
syserror_hander(char *func_name, char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s[%d]: %s %s failed: %s\n", __FILE__, __LINE__, func_name, msg, strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "%s[%d]: %s failed: %s\n", __FILE__, __LINE__, func_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void
usererror_hander(char *func_name, char *msg)
{
    fprintf(stderr, "%s error: %s\n", func_name, msg);
    exit(EXIT_FAILURE);
}

static int
line_count(char *path)
{
    struct stat statbuf;
    struct dirent *dirp;
    char pwd[PATH_MAX];
    char w;
    DIR *dp;
    FILE *fp;

    if (0 != stat(path, &statbuf)) {
        syserror_hander("path", path);
    }

    if (S_ISDIR(statbuf.st_mode)) {
        if (NULL == (dp = opendir(path))) {
            syserror_hander("opendir", path);
        }
        while(NULL != (dirp = readdir(dp))) {
            if (0 == strcmp(dirp->d_name, ".") || 0 == strcmp(dirp->d_name, "..")) {
                continue;
            }
            strcpy(pwd, path);
            if (*(pwd + strlen(pwd) - 1 ) != '/') {
                strcat(pwd, "/");
            }
            strcat(pwd, dirp->d_name);
            if (0 != line_count(pwd)) {
                usererror_hander("line_count", pwd);
            }
        }

        if (closedir(dp) < 0) {
            syserror_hander("closedir", NULL);
        }
    }

    if (S_ISREG(statbuf.st_mode)) {
        if (NULL == (fp = fopen(path, "r"))) {
            syserror_hander("fopen", path);
        }
        while (EOF != (w = fgetc(fp))) {
            if (w == '\n') {
                total_lines++;
            }
        }
        if (0 != fclose(fp)) {
            syserror_hander("fclose", path);
        }
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    int     i;

    if (argc < 2) {
        printf("Usage: %s [PATHNAME]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < argc; i++) {
        if (0 != line_count(argv[i])) {
            usererror_hander("line_count", argv[1]);
        }
    }

    printf("Total lines: %d\n", total_lines);

    return 0;
}

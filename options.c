#include "options.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int parse_options(int argc, char **argv, Options *o)
{
    memset(o, 0, sizeof(*o));
    o->hidden = geteuid() == 0 ? 1 : 0;
    o->quote = isatty(STDOUT_FILENO);
    int ch;
    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A': o->hidden = 1; break;
        case 'a': o->hidden = 2; break;
        case 'c': o->time_kind = 1; break;
        case 'u': o->time_kind = 2; break;
        case 'd': o->directory = 1; o->recursive = 0; break;
        case 'R': o->recursive = 1; o->directory = 0; break;
        case 'F': o->classify = 1; break;
        case 'f': o->unsorted = 1; break;
        case 'h': o->size_unit = 2; break;
        case 'k': o->size_unit = 1; break;
        case 'i': o->inode = 1; break;
        case 'l': o->long_format = 1; o->numeric = 0; break;
        case 'n': o->long_format = 1; o->numeric = 1; break;
        case 'q': o->quote = 1; break;
        case 'w': o->quote = 0; break;
        case 'r': o->reverse = 1; break;
        case 'S': o->size_sort = 1; o->time_sort = 0; break;
        case 's': o->blocks = 1; break;
        case 't': o->time_sort = 1; o->size_sort = 0; break;
        default:
            fprintf(stderr, "usage: myls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            return -1;
        }
    }
    return optind;
}

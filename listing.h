#ifndef LISTING_H
#define LISTING_H
#include "options.h"
#include <sys/stat.h>
#include <stddef.h>

typedef struct {
    char *name;
    char *path;
    struct stat st;
} Entry;

int list_paths(int count, char **paths, const Options *options);
#endif

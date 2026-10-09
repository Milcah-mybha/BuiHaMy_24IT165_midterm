#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct {
    int hidden;                 /* 0: normal, 1: -A, 2: -a */
    int directory, classify, unsorted, inode, long_format, numeric;
    int quote, recursive, reverse, size_sort, blocks, time_sort;
    int time_kind;              /* 0: mtime, 1: ctime, 2: atime */
    int size_unit;              /* 0: BLOCKSIZE, 1: kilobytes, 2: human */
} Options;

int parse_options(int argc, char **argv, Options *options);
#endif

#include "listing.h"
#include "format.h"
#include "sort.h"
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void error_path(const char *path)
{
    fprintf(stderr, "myls: %s: %s\n", path, strerror(errno));
}

static char *join_path(const char *dir, const char *name)
{
    size_t a = strlen(dir), b = strlen(name);
    if (a > SIZE_MAX - b - 2) { errno = ENOMEM; return NULL; }
    char *path = malloc(a + b + 2);
    if (!path) return NULL;
    memcpy(path, dir, a);
    if (a && dir[a-1] != '/') path[a++] = '/';
    memcpy(path + a, name, b + 1);
    return path;
}

static void free_entries(Entry *entries, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].path);
    }
    free(entries);
}

static int add_entry(Entry **entries, size_t *count, size_t *cap,
                     const char *name, const char *path, const struct stat *st)
{
    if (*count == *cap) {
        if (*cap > SIZE_MAX / 2 / sizeof(**entries)) return -1;
        size_t next = *cap ? *cap * 2 : 16;
        Entry *p = realloc(*entries, next * sizeof(**entries));
        if (!p) return -1;
        *entries = p;
        *cap = next;
    }
    Entry *e = &(*entries)[*count];
    e->name = strdup(name);
    e->path = strdup(path);
    if (!e->name || !e->path) { free(e->name); free(e->path); return -1; }
    e->st = *st;
    ++*count;
    return 0;
}

static int visible(const char *name, const Options *o)
{
    if (o->hidden == 2) return 1;
    if (o->hidden == 1) return strcmp(name, ".") && strcmp(name, "..");
    return name[0] != '.';
}

static void print_total(const Entry *entries, size_t count, const Options *o)
{
    uintmax_t blocks = 0;
    for (size_t i = 0; i < count; i++) {
        uintmax_t n = (uintmax_t)entries[i].st.st_blocks;
        blocks = UINTMAX_MAX - blocks < n ? UINTMAX_MAX : blocks + n;
    }
    if (o->size_unit == 2) {
        char size[32];
        human_size(blocks > UINTMAX_MAX / 512 ? UINTMAX_MAX : blocks * 512,
                   size, sizeof(size));
        printf("total %s\n", size);
    } else printf("total %ju\n", block_units(blocks, o));
}

static int list_directory(const char *path, const Options *o, int heading, int *printed)
{
    DIR *dir = opendir(path);
    if (!dir) { error_path(path); return 1; }
    Entry *entries = NULL;
    size_t count = 0, cap = 0;
    int failed = 0;
    struct dirent *item;
    errno = 0;
    while ((item = readdir(dir))) {
        if (!visible(item->d_name, o)) { errno = 0; continue; }
        char *full = join_path(path, item->d_name);
        if (!full) { fprintf(stderr, "myls: out of memory\n"); failed = 1; break; }
        struct stat st;
        if (lstat(full, &st) < 0) { error_path(full); failed = 1; free(full); errno = 0; continue; }
        if (add_entry(&entries, &count, &cap, item->d_name, full, &st) < 0) {
            fprintf(stderr, "myls: out of memory\n"); failed = 1; free(full); break;
        }
        free(full);
        errno = 0;
    }
    if (errno && !item) { error_path(path); failed = 1; }
    if (closedir(dir) < 0) { error_path(path); failed = 1; }
    if (heading) {
        if (*printed) putchar('\n');
        printf("%s:\n", path);
    }
    *printed = 1;
    sort_entries(entries, count, o);
    if (o->long_format || (o->blocks && isatty(STDOUT_FILENO))) print_total(entries, count, o);
    for (size_t i = 0; i < count; i++) print_entry(&entries[i], o);
    if (o->recursive) {
        for (size_t i = 0; i < count; i++) {
            if (!S_ISDIR(entries[i].st.st_mode) || !strcmp(entries[i].name, ".") || !strcmp(entries[i].name, "..")) continue;
            failed |= list_directory(entries[i].path, o, 1, printed);
        }
    }
    free_entries(entries, count);
    return failed;
}

int list_paths(int count, char **paths, const Options *o)
{
    char *dot = ".";
    if (count == 0) {
        paths = &dot;
        count = 1;
    }
    Entry *files = NULL, *dirs = NULL;
    size_t nf = 0, nd = 0, cf = 0, cd = 0;
    int failed = 0;
    for (int i = 0; i < count; i++) {
        struct stat st;
        int result = o->directory ? lstat(paths[i], &st) : stat(paths[i], &st);
        if (result < 0) {
            /* Dangling symbolic links remain listable. */
            if (o->directory || lstat(paths[i], &st) < 0 || !S_ISLNK(st.st_mode)) {
                error_path(paths[i]); failed = 1; continue;
            }
        }
        if (S_ISDIR(st.st_mode) && !o->directory) {
            if (add_entry(&dirs, &nd, &cd, paths[i], paths[i], &st) < 0) goto memory_error;
        } else {
            if (!o->directory && lstat(paths[i], &st) < 0) { error_path(paths[i]); failed = 1; continue; }
            if (add_entry(&files, &nf, &cf, paths[i], paths[i], &st) < 0) goto memory_error;
        }
    }
    sort_entries(files, nf, o);
    sort_entries(dirs, nd, o);
    for (size_t i = 0; i < nf; i++) print_entry(&files[i], o);
    int printed = nf > 0;
    for (size_t i = 0; i < nd; i++)
        failed |= list_directory(dirs[i].path, o, count > 1 || o->recursive, &printed);
    free_entries(files, nf);
    free_entries(dirs, nd);
    return failed;
memory_error:
    fprintf(stderr, "myls: out of memory\n");
    free_entries(files, nf);
    free_entries(dirs, nd);
    return 1;
}

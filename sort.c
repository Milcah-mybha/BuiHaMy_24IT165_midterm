#include "sort.h"
#include <stdlib.h>
#include <string.h>

static const Options *sorting_options;

static struct timespec entry_time(const Entry *entry)
{
#ifdef __NetBSD__
    /* NetBSD exposes the nanosecond timestamps with *timespec names. */
    if (sorting_options->time_kind == 1) return entry->st.st_ctimespec;
    if (sorting_options->time_kind == 2) return entry->st.st_atimespec;
    return entry->st.st_mtimespec;
#else
    if (sorting_options->time_kind == 1) return entry->st.st_ctim;
    if (sorting_options->time_kind == 2) return entry->st.st_atim;
    return entry->st.st_mtim;
#endif
}

static int compare_entries(const void *left, const void *right)
{
    const Entry *a = left, *b = right;
    int cmp = 0;
    if (sorting_options->size_sort) {
        cmp = (a->st.st_size < b->st.st_size) - (a->st.st_size > b->st.st_size);
    } else if (sorting_options->time_sort) {
        struct timespec x = entry_time(a), y = entry_time(b);
        cmp = (x.tv_sec < y.tv_sec) - (x.tv_sec > y.tv_sec);
        if (!cmp) cmp = (x.tv_nsec < y.tv_nsec) - (x.tv_nsec > y.tv_nsec);
    }
    if (!cmp) cmp = strcmp(a->name, b->name);
    return sorting_options->reverse ? -cmp : cmp;
}

void sort_entries(Entry *entries, size_t count, const Options *options)
{
    if (options->unsorted || count < 2) return;
    sorting_options = options;
    qsort(entries, count, sizeof(*entries), compare_entries);
}

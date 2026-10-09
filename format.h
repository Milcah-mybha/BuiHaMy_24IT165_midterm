#ifndef FORMAT_H
#define FORMAT_H
#include "listing.h"
#include <stdint.h>
uintmax_t block_units(uintmax_t blocks, const Options *options);
void human_size(uintmax_t bytes, char *buffer, size_t length);
void print_entry(const Entry *entry, const Options *options);
#endif

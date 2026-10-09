#include "listing.h"
#include "options.h"

int main(int argc, char **argv)
{
    Options options;
    int first = parse_options(argc, argv, &options);
    if (first < 0) return 1;
    return list_paths(argc - first, argv + first, &options);
}

#include "format.h"
#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <inttypes.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <unistd.h>

static uintmax_t unit_size(const Options *o)
{
    if (o->size_unit == 1) return 1024;
    if (o->size_unit == 2) return 512;
    const char *env = getenv("BLOCKSIZE");
    if (!env || !*env) return 512;
    if (!isdigit((unsigned char)*env)) return 512;
    errno = 0;
    char *end;
    uintmax_t n = strtoumax(env, &end, 10);
    if (errno == ERANGE || end == env || n == 0) return 512;
    if (*end && *end != 'B' && *end != 'b') {
        const char *suffixes = "KMGTPE";
        const char *suffix = strchr(suffixes, toupper((unsigned char)*end));
        if (!suffix) return 512;
        for (size_t i = 0; i <= (size_t)(suffix - suffixes); i++) {
            if (n > UINTMAX_MAX / 1024) return 512;
            n *= 1024;
        }
        end++;
    }
    if (*end == 'B' || *end == 'b') end++;
    if (*end) return 512;
    return n;
}

uintmax_t block_units(uintmax_t blocks, const Options *o)
{
    uintmax_t unit = unit_size(o);
    if (blocks > UINTMAX_MAX / 512) return UINTMAX_MAX / unit;
    uintmax_t bytes = blocks * 512;
    return bytes / unit + (bytes % unit != 0);
}

void human_size(uintmax_t n, char *buf, size_t len)
{
    static const char suffix[] = "BKMGTPE";
    double value = (double)n;
    size_t i = 0;
    while (value >= 1024 && i < sizeof(suffix) - 2) { value /= 1024; i++; }
    if (i && value < 10 && value != (uintmax_t)value)
        snprintf(buf, len, "%.1f%c", value, suffix[i]);
    else snprintf(buf, len, "%.0f%c", value, suffix[i]);
}

static void mode_string(mode_t m, char out[11])
{
    out[0] = S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : S_ISCHR(m) ? 'c' :
             S_ISBLK(m) ? 'b' : S_ISFIFO(m) ? 'p' : S_ISSOCK(m) ? 's' : '-';
    const mode_t bits[] = {S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH};
    const char chars[] = "rwxrwxrwx";
    for (int i = 0; i < 9; i++) out[i+1] = m & bits[i] ? chars[i] : '-';
    if (m & S_ISUID) out[3] = m & S_IXUSR ? 's' : 'S';
    if (m & S_ISGID) out[6] = m & S_IXGRP ? 's' : 'S';
    if (m & S_ISVTX) out[9] = m & S_IXOTH ? 't' : 'T';
    out[10] = 0;
}

static void print_name(const char *name, int quote)
{
    if (!quote) { fputs(name, stdout); return; }
    for (const unsigned char *p = (const unsigned char *)name; *p; p++)
        putchar(isprint(*p) ? *p : '?');
}

void print_entry(const Entry *e, const Options *o)
{
    if (o->inode) printf("%ju ", (uintmax_t)e->st.st_ino);
    if (o->blocks) {
        if (o->size_unit == 2) {
            char buf[32];
            uintmax_t blocks = (uintmax_t)e->st.st_blocks;
            human_size(blocks > UINTMAX_MAX / 512 ? UINTMAX_MAX : blocks * 512,
                       buf, sizeof(buf));
            printf("%s ", buf);
        } else printf("%ju ", block_units((uintmax_t)e->st.st_blocks, o));
    }
    if (o->long_format) {
        char mode[11], date[64], size[32], owner[32], group[32];
        mode_string(e->st.st_mode, mode);
        const char *user = owner, *grp = group;
        snprintf(owner, sizeof(owner), "%ju", (uintmax_t)e->st.st_uid);
        snprintf(group, sizeof(group), "%ju", (uintmax_t)e->st.st_gid);
        if (!o->numeric) {
            struct passwd *pw = getpwuid(e->st.st_uid);
            struct group *gr = getgrgid(e->st.st_gid);
            if (pw) user = pw->pw_name;
            if (gr) grp = gr->gr_name;
        }
        if (S_ISCHR(e->st.st_mode) || S_ISBLK(e->st.st_mode))
            snprintf(size, sizeof(size), "%u, %u", major(e->st.st_rdev), minor(e->st.st_rdev));
        else if (o->size_unit == 2) human_size((uintmax_t)e->st.st_size, size, sizeof(size));
        else snprintf(size, sizeof(size), "%jd", (intmax_t)e->st.st_size);
        time_t stamp = o->time_kind == 1 ? e->st.st_ctime : o->time_kind == 2 ? e->st.st_atime : e->st.st_mtime;
        struct tm tm;
        if (localtime_r(&stamp, &tm)) strftime(date, sizeof(date), "%b %e %H:%M", &tm);
        else snprintf(date, sizeof(date), "??? ?? ??:??");
        printf("%s %ju %s %s %s %s ", mode, (uintmax_t)e->st.st_nlink, user, grp, size, date);
    }
    print_name(e->name, o->quote);
    if (o->classify) {
        char mark = S_ISDIR(e->st.st_mode) ? '/' : S_ISLNK(e->st.st_mode) ? '@' :
                    S_ISSOCK(e->st.st_mode) ? '=' : S_ISFIFO(e->st.st_mode) ? '|' :
                    (e->st.st_mode & 0111) ? '*' : 0;
        if (mark) putchar(mark);
    }
    if (o->long_format && S_ISLNK(e->st.st_mode)) {
        size_t cap = e->st.st_size > 0 ? (size_t)e->st.st_size + 2 : 256;
        char *target = NULL;
        for (;;) {
            char *next = realloc(target, cap);
            if (!next) { free(target); break; }
            target = next;
            ssize_t n = readlink(e->path, target, cap - 1);
            if (n < 0) break;
            if ((size_t)n < cap - 1) {
                target[n] = 0;
                fputs(" -> ", stdout);
                print_name(target, o->quote);
                break;
            }
            if (cap > SIZE_MAX / 2) break;
            cap *= 2;
        }
        free(target);
    }
    putchar('\n');
}

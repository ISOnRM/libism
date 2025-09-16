// disk/disk.c
// disk module implementation

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include "disk.h"
#include "util/fld.h"
#include "util/pct_fmt.h"
#include "util/ull_to_human.h"
#include "util/to_pct.h"

// helpre & others

#define PATH_MAX        4096	/* # chars in a path name including nul */

struct majmin { unsigned maj, min; };

static int get_blk_path(char *buf, size_t bufsiz, const char *uuid) {
    if (bufsiz < PATH_MAX) return -1;

    char temp[PATH_MAX];
    snprintf(temp, PATH_MAX, "/dev/disk/by-uuid/%s", uuid);

    char real[PATH_MAX];
    if (!(realpath(temp, real))) return -1;

    int written = snprintf(buf, bufsiz, "%s", real);
    return written;
}

static int get_stat(struct stat *st, const char *blk_path) {
    if (stat(blk_path, st) != 0) return -1;
    return 0;
}

static int get_majmin(struct stat *st, struct majmin *mm) {
    if (!st) return -1;
    if (!S_ISBLK(st->st_mode)) return -1;
    dev_t d = st->st_rdev;
    mm->maj = major(d);
    mm->min = minor(d);
    return 0;
}

static int get_mnt_pt(char *buf, size_t bufsiz, struct majmin mm) {
    if (bufsiz == 0) return -1;

    FILE *fp = fopen("/proc/self/mountinfo", "r");
    if (!fp) return -1;

    char line[8192];
    char mp[4096];

    unsigned mj, mn;

    int result = -1;

    while (fgets(line, sizeof line, fp)) {
        /* flds: 1  2   3(maj:min) 4(root) 5(mount_point) */
        if (sscanf(line, "%*s %*s %u:%u %*s %4095s", &mj, &mn, mp) == 3) {
            if (mj == mm.maj && mn == mm.min) {
                if (strlen(mp) + 1 > bufsiz) { errno = ENOSPC; break; }
                strcpy(buf, mp);
                result = 0;
                break;
            }
        }
    }

    fclose(fp);
    if (result != 0 && errno == 0) errno = ENOENT;
    return result;
}
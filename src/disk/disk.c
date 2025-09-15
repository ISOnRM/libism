// disk/disk.c
// disk module implementation

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include "disk.h"
#include "util/fld.h"
#include "util/pct_fmt.h"
#include "util/ull_to_human.h"
#include "util/to_pct.h"

// helpre & others

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
// disk/disk.c
// disk module implementation

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include "disk.h"
#include "util/fld.h"
#include "util/pct_fmt.h"
#include "util/to_human.h"
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

// final wrapper
static int get_mntpt_byuuid(char *buf, size_t bufsiz, const char *uuid) {
    if (bufsiz == 0 || !uuid) return -1;
    char path[PATH_MAX];
    if (get_blk_path(path, PATH_MAX, uuid) != 0) return -1;
    struct stat st;
    if (get_stat(&st, path) != 0) return -1;
    struct majmin mm;
    if (get_majmin(&st, &mm) != 0) return -1;
    char mnt[PATH_MAX];
    if (get_mnt_pt(mnt, PATH_MAX, mm) != 0) return -1;

    if (!memcpy(buf, mnt, bufsiz)) return -1;

    return 0;
}

static int write_abs(char *buf, size_t bufsiz, char *mntpt, fld_t disk_fld, to_human_cb human_cb) {
    struct statvfs stvfs;
    if (statvfs(mntpt, &stvfs) != 0) return -1;

    // b for blocks s for space
    uintmax_t b_sz    = stvfs.f_frsize;
    uintmax_t s_total = (uintmax_t)stvfs.f_blocks * b_sz;
    uintmax_t s_avail = (uintmax_t)stvfs.f_bavail * b_sz;
    uintmax_t s_used  = s_total - s_avail;

    int written;
    if (disk_fld == TOTAL) {
        written = human_cb(buf, bufsiz, s_total);
    } else if (disk_fld == FREE) {
        written = human_cb(buf, bufsiz, s_avail);
    } else if (disk_fld == USED) {
        written = human_cb(buf, bufsiz, s_used);
    } else return -1;

    return written;
}

static int write_pct(char *buf, size_t bufsiz, char *mntpt, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt) {
    struct statvfs stvfs;
    if (statvfs(mntpt, &stvfs) != 0) return -1;

    // b for blocks s for space
    uintmax_t b_sz    = stvfs.f_frsize;
    uintmax_t s_total = (uintmax_t)stvfs.f_blocks * b_sz;
    uintmax_t s_avail = (uintmax_t)stvfs.f_bavail * b_sz;
    uintmax_t s_used  = s_total - s_avail;

    double field;
    if      (disk_pct_fld == FREE_PCT) field = to_pct(s_avail, s_total);
    else if (disk_pct_fld == USED_PCT) field = to_pct(s_used , s_total);
    else return -1;

    int written;
    if (pct_fmt == PCT_INT) {
        int ipct = (int)(field + .5);
        written = snprintf(buf, bufsiz, "%d", ipct);
    } else if (pct_fmt == PCT_FLOAT) {
        written = snprintf(buf, bufsiz, "%.2f", field);
    } else return -1;

    return written;
}

int get_disk_abs_byuuid(char *buf, size_t bufsiz, char *uuid, fld_t disk_fld, to_human_cb human_cb) {
    if (!uuid || bufsiz == 0) return -1;

    char mntpt[PATH_MAX];
    if (get_mntpt_byuuid(mntpt, PATH_MAX, uuid) != 0) return -1;

    return write_abs(buf, bufsiz, mntpt, disk_fld, human_cb);
}

int get_disk_pct_byuuid(char *buf, size_t bufsiz, char *uuid, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt) {
    if (!uuid || bufsiz == 0) return -1;

    char mntpt[PATH_MAX];
    if (get_mntpt_byuuid(mntpt, PATH_MAX, uuid) != 0) return -1;

    return write_pct(buf, bufsiz, mntpt, disk_pct_fld, pct_fmt);
}

int get_disk_abs_bymntpt(char *buf, size_t bufsiz, char *path, fld_t disk_fld, to_human_cb human_cb) {
    if (!path || bufsiz == 0) return -1;

    return write_abs(buf, bufsiz, path, disk_fld, human_cb);
}

int get_disk_pct_bymntpt(char *buf, size_t bufsiz, char *path, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt) {
    if (!path || bufsiz == 0) return -1;

    return write_pct(buf, bufsiz, path, disk_pct_fld, pct_fmt);
}
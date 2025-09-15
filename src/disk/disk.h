// disk/disk.h
// disk module header

#ifndef DISK_H
#define DISK_H

#include <stddef.h>
#include "util/ull_to_human.h"
#include "util/pct_fmt.h"
#include "util/fld.h"

int get_disk_abs_byuuid(char *buf, size_t bufsiz, char *uuid, fld_t disk_fld);

int get_disk_pct_byuuid(char *buf, size_t bufsiz, char *uuid, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt);

int get_disk_abs_bymntpt(char *buf, size_t bufsiz, char *path, fld_t disk_fld);

int get_disk_pct_bymntpt(char *buf, size_t bufsiz, char *path, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt);

#endif
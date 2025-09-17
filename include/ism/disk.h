// disk/disk.h
// disk module header

#ifndef DISK_H
#define DISK_H

#include <stddef.h>
#include "to_human.h"
#include "pct_fmt.h"
#include "fld.h"

/*
    FREE field here is actually available blocks (blocks available for non-root user),
    USED is calculated with available blocks
*/

int get_disk_abs_byuuid(char *buf, size_t bufsiz, char *uuid, fld_t disk_fld, to_human_cb human_cb);

int get_disk_pct_byuuid(char *buf, size_t bufsiz, char *uuid, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt);

int get_disk_abs_bymntpt(char *buf, size_t bufsiz, char *path, fld_t disk_fld, to_human_cb human_cb);

int get_disk_pct_bymntpt(char *buf, size_t bufsiz, char *path, fld_pct_t disk_pct_fld, pct_fmt_t pct_fmt);

#endif
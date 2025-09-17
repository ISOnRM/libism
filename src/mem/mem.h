// mem/mem.h
// mem module

#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include "util/to_human.h"
#include "util/pct_fmt.h"
#include "util/fld.h"

typedef enum { RAM, SWAP } mem_type_t;

// Write field of memory type to buffer (human readable)
int get_mem_abs(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    fld_t mem_fld,
    to_human_cb human_cb
);

// Write memory type's filed's percent to buffer
int get_mem_pct(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    fld_pct_t mem_pct_fld,
    pct_fmt_t pct_fmt
);

// Write amount of processes to buffer
// int get_mem_proc_amt()ХЪ

#endif
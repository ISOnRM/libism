#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include "util/ull_to_human.h"
#include "util/pct_fmt.h"

typedef enum { RAM, SWAP } mem_type_t;
typedef enum { TOTAL, FREE, USED } mem_fld_t;
typedef enum { FREE_PCT, USED_PTC } mem_pct_fld_t;

// Write field of memory type to buffer (human readable)
int get_mem(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    mem_fld_t mem_fld,
    to_human_cb *human_cb
);

// Write memory type's filed's percent to buffer
int get_mem_pct(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    mem_pct_fld_t mem_pct_fld,
    pct_fmt_t pct_fmt
);

#endif
// mem/mem.c
// mem module implementation

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "mem.h"
#include "util/ull_to_human.h"
#include "util/to_pct.h"

// helpers & other

struct raminfo{
    uint64_t total;
    uint64_t free;
    uint64_t used;
};

struct swapinfo {
    uint64_t total;
    uint64_t free;
};

static uint64_t mem_k(const char *key) {
    if (!key) { errno = EINVAL; return 0; }

    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) { errno = EIO; return 0; }

    char k[64], unit[16] = {0};
    unsigned long long v = 0ULL;
    char want[64];
    snprintf(want, sizeof want, "%s:", key);

    uint64_t out = 0;

    while (fscanf(f, "%63s %llu %15s", k, &v, unit) >= 2) {
        if (strcmp(k, key) == 0 || strcmp(k, want) == 0) {
            out = (uint64_t)v;
            if (unit[0] && strcmp(unit, "kB") == 0) out *= 1024ULL;
            break;
        }
    }

    fclose(f);

    if (out == 0) errno = ENOENT;
    return out;
}

#define saturating_sub(a, b) ((a) > (b) ? ((a) - (b)) : 0)

static int read_raminfo(struct raminfo *ri) {
    uint64_t total   = mem_k("MemTotal");
    uint64_t free_   = mem_k("MemFree");
    uint64_t buffers = mem_k("Buffers");
    uint64_t cached  = mem_k("Cached");
    uint64_t srecl   = mem_k("SReclaimable");
    uint64_t shmem   = mem_k("Shmem");

    uint64_t cache_eff = 0;
    if (cached + srecl >= shmem) cache_eff = (cached + srecl) - shmem;

    uint64_t used = saturating_sub(total, free_);
    used = saturating_sub(used, buffers);
    used = saturating_sub(used, cache_eff);

    ri->total = total;
    ri->free  = free_;
    ri->used  = used;
    return 0;
}

static int read_swapinfo(struct swapinfo *si) {
    if (!si) return -1;
    si->total = mem_k("SwapTotal");
    si->free  = mem_k("SwapFree");
    return 0;
}

int get_mem_abs(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    fld_t mem_fld,
    to_human_cb human_cb
) {
    if (!buf || bufsiz == 0 || !human_cb) return -1;

    int written;
    if (mem_type == RAM) {
        struct raminfo ri;
        read_raminfo(&ri);
        if (mem_fld == TOTAL)     written = human_cb(buf, bufsiz, ri.total);
        else if (mem_fld == FREE) written = human_cb(buf, bufsiz, ri.free);
        else if (mem_fld == USED) written = human_cb(buf, bufsiz, ri.used);
        else return -1;
    } else if (mem_type == SWAP) {
        struct swapinfo si;
        read_swapinfo(&si);
        if (mem_fld == TOTAL)     written = human_cb(buf, bufsiz, si.total);
        else if (mem_fld == FREE) written = human_cb(buf, bufsiz, si.free);
        else if (mem_fld == USED) written = human_cb(buf, bufsiz, si.total - si.free);
        else return -1;
    } else return -1;
    return written;
}


int get_mem_pct(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    fld_pct_t mem_fld_pct,
    pct_fmt_t pct_fmt
) {
    if (!buf || bufsiz == 0) return -1;

    double field;
    if (mem_type == RAM) {
        struct raminfo ri;
        read_raminfo(&ri);
        if      (mem_fld_pct == FREE_PCT) field = to_pct(ri.free, ri.total);
        else if (mem_fld_pct == USED_PCT) field = to_pct(ri.used, ri.total);
        else return -1;
    } else if (mem_type == SWAP) { 
        struct swapinfo si;
        read_swapinfo(&si);
        if      (mem_fld_pct == FREE_PCT) field = to_pct(si.free, si.total);
        else if (mem_fld_pct == USED_PCT) field = to_pct(si.total - si.free, si.total);
        else return -1;
    } else return -1;

    int written;
    if (pct_fmt == PCT_INT) {
        int ipct = (int)(field + .5);
        written = snprintf(buf, bufsiz, "%d", ipct);
    } else if (pct_fmt == PCT_FLOAT) {
        written = snprintf(buf, bufsiz, "%.2f", field);
    } else return -1;
    return written;
}
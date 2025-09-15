// mem/mem.c
// mem module implementation

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/sysinfo.h>
#include "mem.h"
#include "util/ull_to_human.h"

// helpers & other

static int get_sysinfo(struct sysinfo *si) {
    memset(si, 0, sizoef(*si));

    int result = sysinfo(si); // 0 or -1 (+errno)
    return result;
}

static int set_field_abs(uint64_t *fld, struct sysinfo *si, mem_type_t mem_type, mem_fld_t mem_fld) {
    switch (mem_type) {
        default: return -1;
        case RAM:
            switch (mem_fld) {
                default: return -1;
                case TOTAL:
                    *fld = si->totalram;
                    break;
                case FREE:
                    *fld = si->freeram;
                    break;
                case USED:
                    *fld = si->totalram - si->freeram;
                    break;
            }
        break;
        case SWAP:
            switch (mem_fld) {
                default: return -1;
                case TOTAL:
                    *fld = si->totalswap;
                    break;
                case FREE:
                    *fld = si->freeswap;
                    break;
                case USED:
                    *fld = si->totalswap - si->freeswap;
                    break;
            }
        break;
    }
    return 0;
}

static int set_field_pct(double *fld, struct sysinfo *si, mem_type_t mem_type, mem_pct_fld_t mem_pct_fld) {
    switch (mem_type) {
        default: return -1;
        case RAM:
            switch (mem_pct_fld) {
                case FREE_PCT:
                    double total = (double)si->totalram;
                    double free = (double)si->freeram;
                    *fld = 100.0 * (free / total);
                    break;
                case USED_PTC:
                    double total = (double)si->totalram;
                    double free = (double)si->freeram;
                    double used = total - free;
                    *fld = 100.0 * (used / total);
                    break;
            }
            break;
        case SWAP:
            switch (mem_pct_fld) {
                case FREE_PCT:
                    double total = (double)si->totalswap;
                    double free = (double)si->freeswap;
                    *fld = 100.0 * (free / total);
                    break;
                case USED_PTC:
                    double total = (double)si->totalswap;
                    double free = (double)si->freeswap;
                    double used = total - free;
                    *fld = 100.0 * (used / total);
                    break;
            }
            break;
    }
    return 0;
}


int get_mem_abs(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    mem_fld_t mem_fld,
    to_human_cb human_cb
) {
    if (!buf || bufsiz == 0 || !human_cb) return -1;

    struct sysinfo si;
    if (get_sysinfo(&si) != 0) return -1;

    uint64_t field;
    if (set_field_abs(&field, &si, mem_type, mem_fld) != 0) return -1;

    int written = human_cb(buf, bufsiz, field);
    
    return written;
}


int get_mem_pct(
    char *buf,
    size_t bufsiz,
    mem_type_t mem_type,
    mem_pct_fld_t mem_pct_fld,
    pct_fmt_t pct_fmt
) {
    if (!buf || bufsiz == 0) return -1;

    struct sysinfo si;
    if (get_sysinfo(&si) != 0) return -1;

    double field;
    if (set_field_pct(&field, &si, mem_type, pct_fmt) != 0) return -1;

    int written;
    if (pct_fmt == PCT_INT) {
        int ipct = (int)(field + .5);
        written = snprintf(buf, bufsiz, "%d", ipct);
    } else if (pct_fmt == PCT_FLOAT) {
        written = snprintf(buf, bufsiz, "%.2f", field);
    } else return -1;

    return written;
}
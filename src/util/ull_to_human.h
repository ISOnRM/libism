#ifndef ULL_TO_HUMAN_H
#define ULL_TO_HUMAN_H

#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <errno.h>

typedef int (*to_human_cb)(char *, size_t, uint64_t);

// IEC not. - Write human readable memory amount (from bytes) to buffer (use as callback)
static inline int ull_to_human_iec(
    char *buf,
    size_t bufsiz,
    uint64_t num
) {
    if (!buf || bufsiz == 0) {
        errno = EINVAL;
        return -1;
    }

    const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    size_t unit_count = sizeof(units) / sizeof(units[0]);

    size_t i = 0;
    double size = (double)num;
    while (size >= 1024.0 && i < unit_count - 1) {
        size /= 1024.0;
        i++;
    }

    int written = snprintf(buf, bufsiz, "%.2f %s", size, units[i]);
    return written;
}

// SI not. - Write human readable memory amount (from bytes) to buffer (use as callback)
static inline int ull_to_human_si(
    char *buf,
    size_t bufsiz,
    uint64_t num
) {
    if (!buf || bufsiz == 0) {
        errno = EINVAL;
        return -1;
    }

    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    size_t unit_count = sizeof(units) / sizeof(units[0]);

    size_t i = 0;
    double size = (double)num;
    while (size >= 100.0 && i < unit_count - 1) {
        size /= 100.0;
        i++;
    }

    int written = snprintf(buf, bufsiz, "%.2f %s", size, units[i]);
    return written;
}

#endif
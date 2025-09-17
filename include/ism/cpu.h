// cpu/cpu.h
// cpu module header

#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include "pct_fmt.h"

int get_cpu_pct(char *buf, size_t bufsiz, double interval, pct_fmt_t pct_fmt);

// int get_cpu_freq(char *buf, size_t bufsiz);

// int get_cpu_topology

#endif
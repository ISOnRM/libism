// cpu/cpu.h
// cpu module header

#ifndef CPU_H
#define CPU_H

#include "util/pct_fmt.h"

typedef unsigned long size_t;

int get_cpu_pct(char *buf, size_t bufsiz, pct_fmt_t pct_fmt);

int get_cpu_freq(char *buf, size_t bufsiz);

// int get_cpu_topology

#endif
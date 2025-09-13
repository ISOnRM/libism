// cpu/cpu.h
// cpu module header

#ifndef CPU_H
#define CPU_H

typedef unsigned long size_t;

typedef enum { CPU_PCT_INT, CPU_PCT_FLOAT } cpu_pct_fmt_t;

int get_cpu_pct(char *buf, size_t bufsiz, cpu_pct_fmt_t pct_fmt);

int get_cpu_freq(char *buf, size_t bufsiz);

// int get_cpu_topology

#endif
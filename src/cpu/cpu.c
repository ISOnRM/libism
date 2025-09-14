// cpu/cpu.c
// cpu module implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "cpu.h"

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
} cpu_times_t;

// helpers & other

static int read_cpu_times(cpu_times_t *t) {
    FILE *f = fopen("/proc/stat", "r");
    if (!f)
        return -1;

    char line[1024];

    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return -1;
    }

    fclose(f);

    if (strncmp(line, "cpu", 3) != 0) return -1;

    memset(t, 0 ,sizeof(*t));

    char tag[8];
    int n = sscanf(
        line, "%7s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
        tag,
        &t->user,
        &t->nice,
        &t->system,
        &t->idle,
        &t->iowait,
        &t->irq,
        &t->softirq,
        &t->steal,
        &t->guest,
        &t->guest_nice
    );

    if (strcmp(tag, "cpu") != 0)
        return -1;

    if (n < 6)
        return -1;

    return 0;
}



static double usage_from_two(const cpu_times_t *a, const cpu_times_t *b) {
    // I did not write it myself, I took the formula from online
    unsigned long long idle_a = a->idle + a->iowait;
    unsigned long long idle_b = b->idle + b->iowait;

    unsigned long long non_a = a->user + a->nice + a->system + a->irq + a->softirq + a->steal;
    unsigned long long non_b = b->user + b->nice + b->system + b->irq + b->softirq + b->steal;

    unsigned long long tot_a = idle_a + non_a;
    unsigned long long tot_b = idle_b + non_b;

    if (tot_b <= tot_a) return 0.0;
    unsigned long long totald = tot_b - tot_a;
    unsigned long long idled  = (idle_b > idle_a) ? (idle_b - idle_a) : 0;

    double busy = (double)(totald - idled) / (double)totald;
    if (busy < 0.0) busy = 0.0;
    if (busy > 1.0) busy = 1.0;
    return busy * 100.0;
}

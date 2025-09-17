// cpu/cpu.c
// cpu module implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ism/cpu.h>

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
} cpu_times_t;

// helpers & other

static int sleep_double(double interval) {
    if (interval < 0.001)
        interval = 0.001;

    long long ns = (long long)(interval * 1e9);

    struct timespec req = {
        .tv_sec  = (time_t)(ns / 1000000000LL),
        .tv_nsec = (long)(ns % 1000000000LL)
    };

    while (nanosleep(&req, &req) == -1) {
        if (errno != EINTR)
            return -1;
    }

    return 0;
}

static int read_cpu_times(cpu_times_t *t) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
		int err = ferror(fp);
		fclose(fp);
		errno = err;
		return -1;
	}

    char line[1024];

    if (!fgets(line, sizeof(line), fp)) {
        int err = ferror(fp);
        fclose(fp);
        errno = err;
        return -1;
    }

    fclose(fp);

    if (strncmp(line, "cpu", 3) != 0) {
        errno = EPROTO;
        return -1;
    }

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

    if (strcmp(tag, "cpu") != 0) {
        errno = EPROTO;
        return -1;
    }
        

    if (n < 6){
        errno = EPROTO;
        return -1;
    }
        

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

int get_cpu_pct(char *buf, size_t bufsiz, double interval, pct_fmt_t pct_fmt) {
    if (!buf || bufsiz == 0 || interval <= 0.0 ) {
        errno = EINVAL;
        return -1;
    }

    cpu_times_t a, b;
    if (read_cpu_times(&a) != 0) return -1;
    if (sleep_double(interval) != 0) return -1;
    if (read_cpu_times(&b) != 0) return -1;

    double pct = usage_from_two(&a, &b);

    int written = 0;
    if (pct_fmt == PCT_INT) {
        int ipct = (int)(pct + .5);
        written = snprintf(buf, bufsiz, "%d", ipct);
    } else if (pct_fmt == PCT_FLOAT) {
        written = snprintf(buf, bufsiz, "%.2f", pct);
    } else {errno = EINVAL; return -1;}

    if (written < 0 || (size_t)written >= bufsiz) {
        errno = ERANGE;
        return -1;
    }

    return written;
}

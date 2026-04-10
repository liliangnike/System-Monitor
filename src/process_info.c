#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "process_info.h"

static AlertCallback g_alert_cb = NULL;

void proc_set_alert_cb(AlertCallback cb)
{
    g_alert_cb = cb;
}

void init_proc(process_info_t* p, uint32_t pid, const char* name)
{
    if (!p) return;

    memset(p, 0, sizeof(*p));
    p->pid = pid;
    p->state = PROC_RUNNING;
    p->start_time = time(NULL);   // how many seconds from 1970-1-1 till now

    // string copy, only copy destination string size - 1
    // Then set the last string to be ending character '\0'
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
}

void show_proc(const process_info_t* p)
{
    if (!p) return;
    const char *state_str[] = {"RUNNING", "SLEEPING", "STOPPED", "ZOMBIE"};
    printf("[PID %5u]: %-20s state=%-8s, cpu=%5.1f%%, mem=%llu KB\n",
            p->pid,
            p->name,
            state_str[p->state],
            p->cpu_usage,
            (unsigned long long)(p->mem_bytes / 1024));
}

double sample_proc_cpu(uint32_t pid)
{
    // This function is to simulate to sample process cpu usage
    (void)pid;
    // srand() already was called at the beginning of main function
    // rand() % 8000 means return random value in [0, 7999]
    return (double)(rand() % 8000 / 100.0);
}

uint64_t sample_proc_mem(uint32_t pid)
{
    (void) pid;
    return (uint64_t)(rand() % 503 + 10) * 1024 * 1024;     // 10 - 512 Mb
}

void check_proc_thresholds(const process_info_t* p, double cpu_threshold, uint64_t mem_threshold)
{
    if (!p || !g_alert_cb) return;

    if (p->cpu_usage > cpu_threshold) {
        char msg[128];
        snprintf(msg, sizeof(msg), "CPU usage %.1f%% exceeds threshold %.1f%%",
                                    p->cpu_usage, cpu_threshold);
        g_alert_cb(p, msg);
    }
    
    if (p->mem_bytes > mem_threshold) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Memory %llu MB exceeds threshold %llu MB",
                                    (unsigned long long) (p->mem_bytes >> 20), 
                                    (unsigned long long) (mem_threshold >> 20));
        g_alert_cb(p, msg);
    }
}

#ifndef __PROCESS_INFO_HEADER_FILE__
#define __PROCESS_INFO_HEADER_FILE__

#include <stdint.h>
#include <time.h>

/*
 * process_info.h is for C source code.
 * Must tell C++ compiler: The functions is compiled in C language
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PROC_RUNNING = 0,
    PROC_SLEEPING,
    PROC_STOPPED,
    PROC_ZOMBIE
} proc_state_e;

typedef struct {
    uint32_t pid;
    char name[64];  // C-style string
    proc_state_e state;
    double cpu_usage;
    uint64_t mem_bytes;
    time_t start_time;
} process_info_t;

typedef void (*AlertCallback)(const process_info_t* p, const char* msg);
void proc_set_alert_cb(AlertCallback cb);

void init_proc(process_info_t* p, uint32_t pid, const char* name);
void show_proc(const process_info_t* p);
double sample_proc_cpu(uint32_t pid);
uint64_t sample_proc_mem(uint32_t pid);

void check_proc_thresholds(const process_info_t* p, double cpu_threshold, uint64_t mem_threshold);


#ifdef __cplusplus
} // extern "C"
#endif

#endif

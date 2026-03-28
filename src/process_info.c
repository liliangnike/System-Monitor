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

void proc_init(process_info_t* p, uint32_t pid, const char* name)
{
    if (!p) return;

    memset(p, 0, sizeof(p));
    p->pid = pid;
    p->state = PROC_RUNNING;
    p->start_time = time(NULL);   // how many seconds from 1970-1-1 till now

    // string copy, only copy destination string size - 1
    // Then set the last string to be ending character '\0'
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
}


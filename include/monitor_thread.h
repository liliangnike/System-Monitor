#ifndef __MONITOR_THREAD_HEADER_FILE__
#define __MONITOR_THREAD_HEADER_FILE__

#include <thread>
#include "process_info.h"

class MonitorThread 
{
public:
    MonitorThread();
    ~MonitorThread();

    // thread copy is not allowed
    MonitorThread(const MonitorThread&)             = delete;
    MonitorThread& operator=(const MonitorThread&)  = delete;

    // move is ok
    MonitorThread(const MonitorThread&&)            = default;
    MonitorThread& operator=(const MonitorThread&&) = default;

    void start();
    void stop();

private:
    process_info_t proc_;

};

#endif

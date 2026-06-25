#include "monitor_thread.h"

MonitorThread::MonitorThread()
{
}

MonitorThread::~MonitorThread()
{
}

void MonitorThread::start()
{
    // atomic variable write operation: store()
    //                 read operation: load()
    //
    // Be equivalent to if(running_)
    // Use atomic read function load() is better:
    // - Code readability 
    // - Control memory order
    // Default is std::memory_order_seq_cst
    if (running_.load()) return;
    stop_requested_.store(false);
}

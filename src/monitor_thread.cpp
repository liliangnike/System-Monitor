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

    // C++11 introduces std::thread to unify the functions to create thread for both linux and windows OS
    // 1. Normal function, std::thread(function_name, parameters_lists)
    // 2. Class member function, std::thread(&class::function_name, this, parameters), 'this' is must
    thread_ = std::thread(&MonitorThread::thread_loop, this);
}

void MonitorThread::stop()
{
    stop_requested_.store(false);

    // notify all the waiting threads
    sleep_cv_.notify_all();
}

void MonitorThread::join()
{
}

void MonitorThread::thread_loop()
{
}

#ifndef __MONITOR_THREAD_HEADER_FILE__
#define __MONITOR_THREAD_HEADER_FILE__

#include <thread>
#include <atomic>
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

    void join();

    bool is_running() { return running_.load();}

    const std::string& proc_name() const { return proc_name_; }
private:
    void thread_loop();

    process_info_t proc_;
    std::string proc_name_;

    std::atomic<bool> running_{false};
    std::atomic<bool> stop_requested_{false};

    std::thread thread_;

    std::condition_variable sleep_cv_;
};

#endif

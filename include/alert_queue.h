#ifndef __ALERT_QUEUE_HEADER_FILE__
#define __ALERT_QUEUE_HEADER_FILE__

#include <queue>
#include <mutex>
#include "observer.h"

class AlertQueue 
{
public:
    // Producer
    void push(const AlertEvent& event);
private:
    // 1. FIFO
    // 2. Check empty() before using pop()
    // 3. Not thread-safe. If another thread is trying to pop when one thread is pushing, then program will be crashed
    // 4. Try to use emplace(). push() copies objects.
    std::queue<AlertEvent> queue_;
    mutable std::mutex mutex_;      // std::queue is not thread-safe
};

#endif

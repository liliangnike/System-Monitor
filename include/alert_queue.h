#ifndef __ALERT_QUEUE_HEADER_FILE__
#define __ALERT_QUEUE_HEADER_FILE__

#include <queue>
#include <mutex>
#include <condition_variable>
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

    // Thread - Room
    // std::mutex - Door of the room
    // std::condition_variable 
    // 1. Alarm Clock in the room - when some conditions are not met, set a timer and let thread to sleep. Release the lock (open the door)
    // 2. Two-way radio in the room - Whem the conditions are met, the thread will be invoked immediately and locked (close the door)
    std::condition_variable cv_;
};

#endif

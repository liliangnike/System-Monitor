#include "alert_queue.h"

void AlertQueue::push(const AlertEvent& event)
{
    // Define scope of the mutex lock
    {
        std::lock_guard<std::mutex> lock(mutex_);   // mutex_.lock()
        queue_.push(event);
    }
    // RAII, lock destructor (mutex_.unlock() is called automatically)

    cv_.notify_one();   // Call thread "Wake up, please. Works are coming."
}

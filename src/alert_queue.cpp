#include "alert_queue.h"

// Producer thread call push
void AlertQueue::push(const AlertEvent& event)
{
    // Define scope of the mutex lock
    {
        std::lock_guard<std::mutex> lock(mutex_);   // mutex_.lock()
        queue_.push(event);
    }
    // RAII, lock destructor (mutex_.unlock() is called automatically)

    cv_.notify_one();   // Invoke one thread from consumer thread queue - "Wake up, please. Works are coming.". The invoked thread has highest priority.
}

// Consumer thread call pop
std::optional<AlertEvent> AlertQueue::pop()
{
    // During life cycle of std::lock_guard, unlock the mutex is not allowed
    // std::unique_lock allows lock/unlock at any time
    // And condition_variable wait() only use std::unique_lock.
    std::unique_lock<std::mutex> lk(mutex_);

    // The lambda function is to avoid Spurious Wakeup - Even though producer did not call notify_one(), OS might invoke the consumer thread
    cv_.wait(lk, [this]() { return !queue_.empty() || shutdown_; });

    if (queue_.empty()) return std::nullopt;
    AlertEvent event = queue_.front();
    queue_.pop();

    return event;
}

void AlertQueue::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
    }

    cv_.notify_all();
}

bool AlertQueue::empty() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

std::size_t AlertQueue::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

#include "observer.h"
#include "logger.h"
#include <algorithm>

void AlertSubject::subscribe(std::shared_ptr<AlertObserver> observer)
{
    cleanup_expired_observers();
    observers_.emplace_back(observer);
}

void AlertSubject::unsubscribe(const std::string& name)
{
    auto it = std::remove_if(observers_.begin(), observers_.end(),
                            [&name](const std::weak_ptr<AlertObserver>& obs) {
                                auto sp = obs.lock();
                                return !sp || sp->observer_name() == name;
                            });

    observers_.erase(it, observers_.end());
}

void AlertSubject::notify(const AlertEvent& event)
{
    cleanup_expired_observers();
    for(auto& wp : observers_) {
        if (auto sp = wp.lock()) { // lock(): weak_ptr -> shared_ptr
            sp->on_alert(event);
        }
    }
}

void AlertSubject::cleanup_expired_observers()
{
    // remove_if defined in the library algorithm
    auto it = std::remove_if(observers_.begin(), observers_.end(),
                            [](const std::weak_ptr<AlertObserver>& obs){ return obs.expired(); });

    // remove_if does not remove elements.
    // If removal is required, erase function is needed
    observers_.erase(it, observers_.end());
}

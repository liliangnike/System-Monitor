#include "observer.h"
#include "logger.h"
#include <algorithm>

void AlertSubject::subscribe(std::shared_ptr<AlertObserver> observer)
{

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

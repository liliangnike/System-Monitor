#include <algorithm>
#include <iostream>
#include "observer.h"
#include "logger.h"

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

void ConsoleAlertObserver::on_alert(const AlertEvent& event)
{
    // conditional operator should be faster
    /*const char* type_str =
        (event.alert_type == AlertType::CPU_HIGH)  ? "CPU HIGH " :
        (event.alert_type == AlertType::MEM_HIGH)  ? "MEM HIGH " :
        (event.alert_type == AlertType::CPU_NORMAL)? "CPU OK   " : "MEM OK   ";
    */

    std::string type_str;
    switch(event.alert_type)
    {
        case AlertType::CPU_HIGH:
            type_str = "CPU HIGH ";
            break;
        case AlertType::MEM_HIGH:
            type_str = "MEM HIGH ";
            break;
        case AlertType::CPU_NORMAL:
            type_str = "CPU OK   ";
            break;
        default:
            type_str = "MEM OK   ";
            break;
    }

    std::cout << "    [ALERT][" << type_str << "] "
              << event.proc.name
              << " - " << event.message << std::endl;
}

void LogAlertObserver::on_alert(const AlertEvent& event)
{
    bool is_warning = (event.alert_type == AlertType::CPU_HIGH || event.alert_type == AlertType::MEM_HIGH);
    std::string msg = "[Observer] " + event.message;
    if (is_warning) {
        Logger::instance()->warn(msg);
    }
    else {
        Logger::instance()->info(msg);
    }
}

void StatsAlertObserver::on_alert(const AlertEvent& event)
{
    if (event.alert_type == AlertType::CPU_HIGH) ++cpu_count_;
    if (event.alert_type == AlertType::MEM_HIGH) ++mem_count_;
}


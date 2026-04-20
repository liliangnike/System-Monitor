#ifndef __OBSERVER_HEADER_FILE__
#define __OBSERVER_HEADER_FILE__

#include <memory>
#include "process_info.h"

enum class AlertType {
    CPU_HIGH,
    MEM_HIGH,
    CPU_NORMAL,
    MEM_NORMAL
};

struct AlertEvent {
    AlertType alert_type;
    process_info_t proc;
    std::string message;
    double value;
    double threshold;
};

// abstract observer interface
class AlertObserver {
public:
    // Must be virtual destructor, if the parent class was used as unified interface
    virtual ~AlertObserver() = default;
    virtual std::string observer_name() const;

};

class AlertSubject {
public:
    // generic inteface for external caller function.
    // C++ allows implicit conversion shared_ptr -> weak_ptr, when function is pushing shared_ptr type data back to a vector of weak_ptr, the type is converted implicitly.
    void subscribe(std::shared_ptr<AlertObserver> observer);
    void unsubscribe(const std::string& name);
    void notify(const AlertEvent& event);

private:
    std::vector<std::weakr_ptr<AlertObserver>> observers_;
};

class ConsoleAlertObserver : public AlertObserver {
public:
    std::string observer_name() const override { return "ConsoleObserver"; }
};

class LogAlertObserver : public AlertObserver {
public:
    std::string observer_name() const override { return "LogObserver"; }
};

#endif

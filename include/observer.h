#ifndef __OBSERVER_HEADER_FILE__
#define __OBSERVER_HEADER_FILE__

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
    void subscribe();
    void unsubscribe(const std::string& name);
    void notify();
};

#endif

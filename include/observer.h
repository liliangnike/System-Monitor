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
    virtual ~AlertObserver() = default;

};

#endif

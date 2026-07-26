#ifndef __OBSERVER_HEADER_FILE__
#define __OBSERVER_HEADER_FILE__

#include <string>
#include <vector>
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
    virtual std::string observer_name() const = 0;
    virtual void on_alert(const AlertEvent& event) = 0;

};

class AlertSubject {
public:
    // generic inteface for external caller function, user shared_ptr here.
    // C++ allows implicit conversion shared_ptr -> weak_ptr, when function is pushing shared_ptr type data back to a vector of weak_ptr, the type is converted implicitly.
    void subscribe(std::shared_ptr<AlertObserver> observer);
    void unsubscribe(const std::string& name);
    void notify(const AlertEvent& event);

    std::size_t observer_size() const;

    void cleanup_expired_observers();

private:
    // Define weak_ptr type.
    // Why not using shared_ptr directly?
    // For example, in main function
    // 1. stats_obs -> counter is 1
    // 2. cpu_mon -> subscribe -> counter + 1 = 2
    // 3. After main function execution, stats_obs is local and destroyed automatically. counter - 1 = 1
    // 4. StatsAlertObserver will not be freed because counter is 1 now. Memory leak occurs.
    // 5. weak_ptr does not have counter, so such issue
    //
    // In observer pattern, subject should not "own" life cycle of observers. So weak_ptr type should be used.
    std::vector<std::weak_ptr<AlertObserver>> observers_;
};

// print to console
class ConsoleAlertObserver : public AlertObserver {
public:
    void on_alert(const AlertEvent& event) override;
    std::string observer_name() const override { return "ConsoleObserver"; }
};

// write into log
class LogAlertObserver : public AlertObserver {
public:
    void on_alert(const AlertEvent& event) override;
    std::string observer_name() const override { return "LogObserver"; }
};

// Statistics alerts
class StatsAlertObserver : public AlertObserver {
public:
    void on_alert(const AlertEvent& event) override;
    std::string observer_name() const override { return "StatsObserver"; }

    int get_cpu_alert_count() const { return cpu_count_; }
    int get_mem_alert_count() const { return mem_count_; }

private:
    int cpu_count_ = 0;
    int mem_count_ = 0;
};

#endif

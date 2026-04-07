#include <iostream>
#include <cstdlib>
#include <vector>

#include "logger.h"
#include "monitor.h"
#include "process_info.h"

static void alert_callback(const process_info_t* p, const char* msg)
{
    Logger::instance()->warn(std::string("[ALERT] ") + msg + " (proc=" + p->name + ")");
}

int main(void)
{
    std::srand(42);

    // Logger singleton instance
    Logger* log = Logger::instance();
    log->set_loglevel(LogLevel::DEBUG);
    log->set_log_file("system_monitor.log");
    
    log->info("=== System Monitor Started ===");

    proc_set_alert_cb(alert_callback);

    const char* proc_names[] = {
        "nginx", "postgres", "redis", "python3", "gcc"
    };
    constexpr int PROC_COUNT = 5;   // const means the variable is not permitted to be changed during programming execution
                                    // constexpr means that the variable can be calculated out during compilation phase

    std::vector<process_info_t> procs(PROC_COUNT);
    for (int i = 0; i < PROC_COUNT; i++) {
        init_proc(&procs[i], 1000 + i * 100, proc_names[i]);
    }

    // Factory function to create monitors
    std::vector<std::unique_ptr<MonitorBase>> monitors;
    monitors.push_back(MonitorFactory::create(MonitorFactory::Type::CPU));
    monitors.push_back(MonitorFactory::create(MonitorFactory::Type::MEMORY));
    monitors.push_back(MonitorFactory::create("composite"));

    log->info("Created " + std::to_string(monitors.size()) + " monitors via factory.");

    log->info("=== Monitoring round ===");
    /*
     * C++ auto keyword:
     * 1. auto elem:vector -> value copy and does not modify value in the vector
     * 2. auto& elem:vector -> no value copy and could modify value in the vector (reference)
     * 3. const auto& elem:vector -> no value copy and could not modify value in the vector
     */
    for (auto& proc:procs) {
        for (auto& monitor:monitors) {
            try {
                // Call base class virtual function
                // During program run, associate child class function will be called
                monitor->run(proc);
            } catch (const std::exception& e) {
                log->error("Monitor threw: " + std::string(e.what()));
            }
        }
        
        // C function
        check_proc_thresholds(&proc, 60.0, 300ULL * 1024 * 1024);
    }

    log->info("=== Show Process Information ===");
    for (const auto& proc:procs) {
        // C function
        show_proc(&proc);
    }

	return 0;
}

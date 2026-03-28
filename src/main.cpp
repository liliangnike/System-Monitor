#include <iostream>
#include <cstdlib>

#include "logger.h"

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
    for (i = 0; i < PROC_COUNT; i++) {
        proc_init(&procs[i], 1000 + i * 100, proc_names[i]);
    }

	return 0;
}

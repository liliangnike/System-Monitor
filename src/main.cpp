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

static void demo_stl(const std::vector<process_info_t>& procs) 
{
    Logger::instance()->info("=== STL, Lambda demo ===");
    
    // lambda expression (function)
    // syntax: [capture list](parameter list) -> return value type { function body }
    //
    // C++ compiler consider lambda as one class
    // capture list: data member (normally they are external variables required in the lambda function)
    // parameter list: member function parameter list
    // return type: return data type, optional (think about Python)
    // function body: implementation
    //
    auto high_cpu = [](const process_info_t& p){ return p.cpu_usage > 50.0; };

    int count = 0;
    for (const auto& p : procs) {
        if (high_cpu(p)) count++;
    }

    Logger::instance()->info("Number of processes with CPU usage exceeds 50.0%: " + std::to_string(count));
}

static void demo_memory()
{
    // demo usage of point, reference and smart pointer
    Logger::instance()->info("=== Memory demo ===");

    // stack - local variable
    process_info_t stack_proc;
    init_proc(&stack_proc, 9999, "stack_demo");

    // heap - new
    process_info_t* heap_proc = new process_info_t;
    init_proc(heap_proc, 8888, "pointer_demo");
    /*
     * If delete operator throws exceptions, what will happen?
     * 1. delete calls destructor function then call operator delete to free memory
     * 2. C++, when stack unwinding is inprogress, if there is exceptions, calls std::terminate(). Then code will crash. Memory might not be free.
     * 3. Even though try...catch is used, no exception will be caught. Because std::terminate() is called.
     *
     * Never try to raise exception in C++ destructor function.
     */
    delete heap_proc;

    // unique_ptr - safe, free automatically
    auto smart = std::make_unique<process_info_t>();
    init_proc(smart.get(), 7777, "smart_pointer_demo");
    
    Logger::instance()->debug("stack process id = " + std::to_string(stack_proc.pid));
    Logger::instance()->debug("smart process id = " + std::to_string(smart->pid));
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

    demo_stl(procs);

    demo_memory();

    // Demo code for exception threw by MonitorFactory function
    try {
        auto unknown_monitor = MonitorFactory::create("unknown_type");
    } catch (const std::invalid_argument& e) {
        log->warn("Expected exception caught during monitor creation: " + std::string(e.what()));
    }

    log->info("=== Finished. Total log messages: " + std::to_string(log->get_message_count()) + " ===");

	return 0;
}

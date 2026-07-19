#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <atomic>

#include "logger.h"
#include "monitor.h"
#include "process_info.h"
#include "observer.h"
#include "snapshot_writer.h"
#include "alert_queue.h"
#include "monitor_thread.h"

// CPU hardware layer instruction (x86:LOCK)
// If one thread is executing atomic operations, it will not be interrupted
// Example,
// 1. int count = 0; count++; : read count -> +1 -> write value to memory. When another thread writes the value, the value might be incorrect.
// 2. std::atomic<int> count{0}; count++; : CPU merges all steps into "one step", no any interruptions during threads "data race"
//
// For multiple threads,
// - flags, state, counters, .etc: std::atomic should be used, much faster than mutex
// - complex code logic: std::mutex should be used
static std::atomic<uint64_t> g_sample_count{0};

class QueueAlertObserver : AlertObserver
{
public:
    explicit QueueAlertObserver(AlertQueue& q) : queue_(q) {}
    void on_alert(const AlertEvent& event) override
    {
        queue_.push(event);
        // std::atomic add function
        // fetch -> then add
        // return the old value before adding
        // std::memory_order_relaxed: only atomic safety, do not care about other code execution order
        // memory order (from loose to strict): relaxed < acquire/release < seq_cst
        g_sample_count.fetch_add(1, std::memory_order_relaxed);
    }

    std::string observer_name() const override { return "QueueObserver"; }

private:
    AlertQueue& queue_;
};

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
    // capture list: data member (normally they are external variables required in the lambda function) - That's why, lamda functions with data members could not be converted into C function pointer, because C function pointer is address, no space to save data members (capture list); if the capture list is empty, then lambda can be converted into C function pointer.
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

    /* ----------------------------------------------------------
     * 1. Initial logger instance - Singleton
     * ----------------------------------------------------------*/

    Logger* log = Logger::instance();
    log->set_loglevel(LogLevel::DEBUG);
    log->set_log_file("system_monitor.log");
    
    log->info("=== System Monitor v3 Multi-threaded ===");

    AlertQueue alert_queue;

    /* ----------------------------------------------------------
     * 2. Create observers (shared_ptr)
     * ----------------------------------------------------------*/
    auto console_obs = std::make_shared<ConsoleAlertObserver>();
    auto log_obs     = std::make_shared<LogAlertObserver>();
    auto statis_obs  = std::make_shared<StatsAlertObserver>();
    auto queue_obs   = std::make_shared<QueueAlertObserver>(alert_queue);

    /* ----------------------------------------------------------
     * 3. Create monitors by factory
     * ----------------------------------------------------------*/
    auto mem_mon = MonitorFactory::create(MonitorFactory::Type::MEMORY);
    auto composite_mon = MonitorFactory::create("composite");

    /* ----------------------------------------------------------
     * 4. Subscribe observers to Subject (Monitor)
     *    observers are saved as weak_ptr type
     * ----------------------------------------------------------*/

    mem_mon->subscribe(queue_obs);
    mem_mon->subscribe(console_obs);
    mem_mon->subscribe(statis_obs);

    /* ----------------------------------------------------------
     * 5. Register alert callback function
     * ----------------------------------------------------------*/
    proc_set_alert_cb(alert_callback);

    /* ----------------------------------------------------------
     * 6. Simulate processes list
     * ----------------------------------------------------------*/
    const char* proc_names[] = {
        "nginx", "postgres", "redis", "python3", "gcc"
    };
    constexpr int PROC_COUNT = 5;   // const means the variable is not permitted to be changed during programming execution
                                    // constexpr means that the variable can be calculated out during compilation phase

    // Why unique_ptr here (not thread objects)?
    // MonitorThread contains std::thread that could not be copied
    // When vector inserts new elements, existing elements need to be moved, and there are restrictions on mutex/condition_variable objects move
    // unique_ptr just pointer move. It is safe.
    std::vector<std::unique_ptr<MonitorThread>> threads;

    for (int i = 0; i < PROC_COUNT; i++) {
        process_info_t proc;
        init_proc(&proc, 1000 + i * 100, proc_names[i]);

        auto cpu_mon = MonitorFactory::create(MonitorFactory::Type::CPU);
        cpu_mon->subscribe(queue_obs);
        cpu_mon->subscribe(console_obs);
        cpu_mon->subscribe(statis_obs);

        threads.push_back(std::make_unique<MonitorThread>(proc, std::move(cpu_mon), std::chrono::milliseconds(300)));
    }

    // lambda function as thread function
    // AlertQueue contains mutex and condition_variable. Both could not be copied. So use reference in capture list
    // csv_writer_thread is sub-thread
    std::thread csv_writer_thread( [&alert_queue]() {
        log->info("[CSVWriter] Thread started");
        SnapshotWriter writer("alerts.csv");
        while(true) {
            auto event = alert_queue.pop();
            if (!event.has_value()) break;  //nullopt means 'shutdown', according to pop() implementation
            writer.write(event->proc);
        }
    } );
  
    // csv_writer_thread: pop() → wait() → 挂起
    // MonitorThread[nginx]:   采样 → push() → notify → csv_writer 醒来写CSV → 再次挂起
    // MonitorThread[postgres]: 采样 → push() → notify → csv_writer 醒来写CSV → 再次挂起
    // 主线程 sleep 2秒后
    // → alert_queue.shutdown()
    // → shutdown_ = true，notify_all()
    // → csv_writer 被唤醒
    // → predicate 为 true（shutdown_ == true）
    // → pop() 返回 nullopt
    // → csv_writer_thread 退出循环，线程结束

    log->info("Starting " + std::to_string(PROC_COUNT) + "monitor threads..." );
    for (auto &t : threads) t->start();

    log->info("Monitoring for 2 seconds...");
    std::this_thread::sleep_for(std::chrono::seconds(2));

    log->info("Stopping monitor threads...");
    for(auto &t : threads) t->stop();

    log->info("Shutting down alert queue...");
    alert_queue.shutdown();

    log->info("Waiting for CSV writer thread to finish...");
    csv_writer_thread.join();

    /* ----------------------------------------------------------
     * Observer Statistics
     * ----------------------------------------------------------*/
    log->info("=== Results ===");
    log->info("Total alerts: " + std::to_string(g_sample_count.load()));
    log->info("CPU alerts: " + std::to_string(statis_obs->get_cpu_alert_count()));
    log->info("MEM alerts: " + std::to_string(statis_obs->get_mem_alert_count()));

    /* ----------------------------------------------------------
     * Flush history data into csv file
     * ----------------------------------------------------------*/
    log->info("=== Flush history to CSV ===");
    {
        std::string csv_filename("snapshot.csv");
        SnapshotWriter writer(csv_filename);


        if (writer.is_open()) {
            for (auto &t : threads) {
                writer.flush_history(t->monitor().get_history());
            }
            log->info("Wrote " + std::to_string(writer.written_rows()) + " rows to " + csv_filename);
        }
    }


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

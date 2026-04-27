#ifndef __MONITOR_HEADER_FILE__
#define __MONITOR_HEADER_FILE__

#include <memory>
#include <cstdint>

#include "process_info.h"
#include "observer.h"

class MonitorBase : public AlertSubject {
public:
    MonitorBase() = default;
    virtual ~MonitorBase() = default;   // Hey Compiler. Please help to generate the default destructor functon.
                                        // Also think about the keyword 'delete'
                                        // In parent class, destructor must be virtual.
   
    // Pure virtual functions
    // Must implemented in Child classes
    virtual std::string name() const = 0;
    virtual void collect(process_info_t& proc) = 0;
    virtual std::string report(const process_info_t& proc) const = 0;

    void run(process_info_t& proc);

    // history size belongs to class instead of objects
    // constexpr means this variable should be initialized in class. So it can be size of array defined in the class
    // 'static const' does not allow users to assign value in class definition. User needs to give value in .cpp file
    static constexpr std::size_t HISTORY_SIZE = 20;
};

// final means that the inheritance 'stops' here, the child class is prohibitted to derive (impossible to have its child class)
class CpuMonitor final : public MonitorBase {
public:
    explicit CpuMonitor(double warn_threshold = 70.0);

    std::string name() const override;
    void collect(process_info_t& proc) override;
    std::string report(const process_info_t& proc) const override;
private:
    double warn_threshold_;
};

class MemoryMonitor final : public MonitorBase {
public:
    explicit MemoryMonitor(uint64_t warn_bytes =  256ULL * 1024 * 1024);

    std::string name() const override;
    void collect(process_info_t& proc) override;
    std::string report(const process_info_t& proc) const override;
private:
    uint64_t warn_bytes_;
};

class CompositeMonitor final : public MonitorBase {
public:
    CompositeMonitor();

    std::string name() const override;
    void collect(process_info_t& proc) override;
    std::string report(const process_info_t& proc) const override;
private:
    CpuMonitor cpu_;
    MemoryMonitor memory_;
};

class MonitorFactory {
public:
    // No need to instantiation. Member functions could be 'static' type
    MonitorFactory() = delete;

    enum class Type {CPU, MEMORY, COMPOSITE};

    static std::unique_ptr<MonitorBase> create(Type type);
    static std::unique_ptr<MonitorBase> create(const std::string& type_str);  // pass by reference (no copy). What's the difference with 'pass by value'?
};

#endif

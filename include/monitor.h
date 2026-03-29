#ifndef __MONITOR_HEADER_FILE__
#define __MONITOR_HEADER_FILE__

#include <memory>
#include <cstdint>

class MonitorBase {
public:
    MonitorBase() = default
    virtual ~MonitorBase() = default;   // Hey Compiler. Please help to generate the default destructor functon.
                                        // Also think about the keyword 'delete'
                                        // In parent class, destructor must be virtual.
    
};

// final means that the inheritance 'stops' here, the child class is permitted to derive (impossible to have its child class)
class CpuMonitor final : public MonitorBase {
public:
    explicit CpuMonitor(double warn_threshold = 70.0);
private:
    double warn_threshold_;
};

class MemoryMonitor final : public MonitorBase {
public:
    explicit MemoryMonitor(uint64_t warn_bytes =  256ULL * 1024 * 1024);
private:
    uint64_t warn_bytes_;
};

class CompositeMonitor final : public MonitorBase {
public:
    CompositeMonitor();
private:
    CpuMonitor cpu_;
    MemoryMonitor memory_;
};

class MonitorFactory {
public:
    // No need to instantiation. Member functions could be 'static' type
    MonitorFactory() = delete;

    enum class Type = {CPU, MEMORY, COMPOSITE};

    static std::unique_ptr<MonitorBase> create(Type type);
    static std::unique_ptr<MonitorBase> create(const std:string& type_str);  // pass by reference (no copy). What's the difference with 'pass by value'?
};

#endif

#include <sstream>
#include <iomanip>
#include "logger.h"
#include "monitor.h"

void MonitorBase::run(process_info_t& proc)
{
    collect(proc);
    std::string res = report(proc);
    Logger::instance()->info(res);
}

CpuMonitor::CpuMonitor(double warn_threshold)
    : warn_threshold_(warn_threshold) {}

std::string CpuMonitor::name() const{
    return "CpuMonitor";
}

void CpuMonitor::collect(process_info_t& proc)
{
    proc.cpu_usage = sample_proc_cpu(proc.pid);
    if (proc.cpu_usage > warn_threshold_) {
        Logger::instance()->warn("High CPU: " + name() + ", pid = " + std::to_string(proc.pid));
    }
}

std::string CpuMonitor::report(const process_info_t& proc) const
{
    std::ostringstream oss;
    oss << "[CPU ] pid = " << proc.pid
        << " name = " << proc.name
        << " cpu = " << std::fixed << std::setprecision(1)
        << proc.cpu_usage << "%";

    return oss.str();
}

MemoryMonitor::MemoryMonitor(uint64_t warn_bytes)
    : warn_bytes_(warn_bytes) {}

std::string MemoryMonitor::name() const{
    return "MemoryMonitor";
}

void MemoryMonitor::collect(process_info_t& proc)
{
    proc.mem_bytes = sample_proc_mem(proc.pid);
    if (proc.mem_bytes > warn_bytes_) {
        Logger::instance()->warn("High MEM: " + name() + ", pid = " + std::to_string(proc.pid));
    }
}

std::string MemoryMonitor::report(const process_info_t& proc) const
{
    std::ostringstream oss;
    oss << "[MEM ] pid = " << proc.pid
        << " name = " << proc.name
        << " mem = " << (proc.mem_bytes >> 20) << " MB";

    return oss.str();
}

CompositeMonitor::CompositeMonitor()
    : cpu_(70.0), memory_(256ULL * 1024 * 1024) {}

std::string CompositeMonitor::name() const {
    return "CompositeMonitor";
}

void CompositeMonitor::collect(process_info_t& proc)
{
    cpu_.collect(proc);
    memory_.collect(proc);
}

std::string CompositeMonitor::report(const process_info_t& proc) const
{
    return cpu_.report(proc) + "  |  " + memory_.report(proc);
}

// No need to add 'static' keyword for the static function implementation
std::unique_ptr<MonitorBase>
MonitorFactory::create(MonitorFactory::Type type)   // Don't forget MonitorFactory before 'Type', because Type is defined in the class MonitorFactory
{
    switch(type) {
        case Type::CPU:
            return std::make_unique<CpuMonitor>();
        case Type::MEMORY:
            return std::make_unique<MemoryMonitor>();
        case Type::COMPOSITE:
            return std::make_unique<CompositeMonitor>();
    }
    throw std::invalid_argument("MonitorFactory: Unknown type");
}

std::unique_ptr<MonitorBase>
MonitorFactory::create(const std::string& type_str)
{
    if(type_str == "cpu") return create(Type::CPU);
    else if(type_str == "memory") return create(Type::MEMORY);
    else if(type_str == "composite") return create(Type::COMPOSITE);
    else throw std::invalid_argument("MonitorFactory: Unknown type string: " + type_str);
}

#include <ctime>
#include <iomanip>

#include "snapshot_writer.h"
#include "logger.h"

SnapshotWriter::SnapshotWriter(const std::string& filepath)
    : filepath_(filepath)
{
    file_.open(filepath, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        Logger::instance()->error("SnapshotWriter: cannot open " + filepath);
        return;
    }

    // ofstream seekp - seek position (offset) based on one reference place. 
    // e.g. seekp(5, std::ios::beg), from the beginning, the 5th position
    file_.seekp(0, std::ios::end); // jump to end of the ofstream
    if (file_.tellp() == 0) {       // tellp is to get current 'position' - Bytes number from the file stream beginning
        write_header();
    }

    Logger::instance()->info("SnapshotWriter: Opened " + filepath);
}

SnapshotWriter::~SnapshotWriter()
{
    if (file_.is_open()) {
        file_.flush();
        file_.close();
    }
}

void SnapshotWriter::write_header()
{
    file_ << "timestamp,pid,name,state,cpu_pct,mem_mb" << "\n";
}

void SnapshotWriter::write(const process_info_t& proc)
{
    if (!file_.is_open()) return;

    // format timestamp
    char ts[32];
    std::time_t now_in_seconds = std::time(nullptr);
    std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", std::localtime(&now_in_seconds)); //Both parameter and return types of std::localtime are pointer

    const char* states[] = {"RUNNING", "SLEEPING", "STOPPED", "ZOMBIE"};
    file_ << ts
          << ", " << proc.pid
          << ", " << proc.name
          << ", " << states[proc.state]
          << ", " << std::fixed << std::setprecision(1) << proc.cpu_usage
          << ", " << (proc.mem_bytes >> 20)  // bytes -> Mb
          << "\n";

    ++row_;
    // flush every 10 lines
    if (row_ % 10 == 0) file_.flush();
}


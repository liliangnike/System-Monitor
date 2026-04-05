#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include "logger.h"

Logger* Logger::inst_ = nullptr;

Logger::Logger()
{}

Logger::~Logger()
{}

Logger* Logger::instance()
{
    if(inst_ == nullptr)
    {
        inst_ = new Logger();
    }

    return inst_;
}


void Logger::set_loglevel(LogLevel level)
{
    // Just start reviewing C++ knowledge
    // This is template
    // Take vector as example
    // If you want to initialize a vector with size 10
    // std::vector<int> v(10)
    std::lock_guard<std::mutex> lock(mutex_);

    min_level_ = level;
}

void Logger::set_log_file(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.close();
    }

    /*
    std::ios::in       // read
    std::ios::out      // write
    std::ios::app      // append
    std::ios::trunc    // 清空再写（ofstream 默认行为）
    std::ios::binary   // 二进制模式
     */

    file_.open(path, std::ios::app);
}

void Logger::debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
void Logger::info(const std::string& msg) { log(LogLevel::INFO, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::WARNING, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }
void Logger::critical(const std::string& msg) { log(LogLevel::CRITICAL, msg); }

// Already tell compiler, this is class member function
// No need to add 'static'
std::string Logger::get_level_name(LogLevel level)
{
    switch(level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
    }

    return "UNKNOWN";
}

std::string Logger::get_current_timestamp()
{
    // using namespace std::chrono;

    auto now = std::chrono::system_clock::now();
    // to time_t format
    auto tt = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&tt), "%H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

void Logger::log(LogLevel level, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (level < min_level_) return;

    std::string line = "[" + get_current_timestamp() + "][" + get_level_name(level) + "]" + msg;

    std::cout << line << "\n";              // no flush
    // std::cout << line << std::endl;      // every std::endl calls flush()

    if (file_.is_open()) {
        file_ << line << "\n";
        file_.flush();  // Tell computer, not wait, write the line immediately. Or, just use std::endl directly
    }

    ++count_;
}

uint64_t Logger::get_message_count() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

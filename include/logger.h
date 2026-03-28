#ifndef __LOGGER_HEADER_FILE__
#define __LOGGER_HEADER_FILE_

#include <mutex>
#include <fstream>
#include <cstdint>

// What's the difference between C enum and C++ enum?
// char is to tell C++ compiler that the enum takes up 1 byte
enum class LogLevel : char {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
}

class Logger {
public:
    static Logger *instance();

    // & - Left value reference
    //
    // Logger should not be cloned
    Logger(const Logger&)              = delete;
    // should not be assignable
    Logger& operator=(const Logger&)   = delete;

    // && - Right value reference
    Logger(Logger&&)                   = delete;
    Logger&& operator=(Logger&&)       = delete;


    void set_loglevel(LogLevel level);

    void set_log_file(const std::string& path);

    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

    uint64_t get_message_count() const;

private:
    // Private constructor to avoid new by external instance
    Logger();
    ~Logger();

    static std::string get_level_name(LogLevel level);
    static std::string get_current_timestamp();

    void log(LogLevel level, const std::string& msg);

    static Logger *inst_;

    LogLevel min_level_;

    // declare as mutable. Add lock indeed makes mutex to be changed.
    // For const member function, it is not permitted to change data memebers.
    // Need declare as mutable type
    mutable std::mutex mutex_;

    std::ofstream file_;

    uint64_t count_;
};
#endif
